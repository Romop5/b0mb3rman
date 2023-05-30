#include <bm/game_controller.hpp>

using namespace bm;

namespace {
struct pair_hash
{
  template<class T1, class T2>
  std::size_t operator()(const std::pair<T1, T2>& pair) const
  {
    return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
  }
};
} // namespace

auto
GameController::handle(const event::GameStarted& event) -> void
{
  using namespace bm::game_logic;

  event_distributor_.clear();
  world_.clear();
  world_.create(Entity::Type::player)
    .set_collision_mask_bit(Entity::Type::player)
    .set_collision_mask_bit(Entity::Type::bomb)
    .set_collision_mask_bit(Entity::Type::crate)
    .set_tileset("characters/farmer.json")
    .set_tile(2)
    .set_origin({ 5, 0 })
    .set_size({ 0.7, 0.7 })
    .set_max_speed(10.0f)
    .set_data(PlayerData{});

  world_.create(Entity::Type::npc)
    .set_flags(Entity::Flags::animated_movement)
    .set_collision_mask_bit(Entity::Type::player)
    .set_collision_mask_bit(Entity::Type::bomb)
    .set_collision_mask_bit(Entity::Type::crate)
    .set_tileset("characters/farmer.json")
    .set_tile(2)
    .set_origin({ 5, 0 })
    .set_size({ 0.7, 0.7 })
    .set_max_speed(5.0f)
    .set_data(NPCData{ NPCState::chasing_target, *world_.get_player_id() });

  if (auto level = game_.get_current_level()) {
    const auto boxes_layer_name = "boxes";
    if (not level->map_->has_layer(boxes_layer_name)) {
      spdlog::error("Missing layer {} that defines positions of boxes!",
                    boxes_layer_name);
    } else {
      const auto& layer = level->map_->get_layer(boxes_layer_name);
      if (std::holds_alternative<render::TiledMap::TileLayer>(layer.data_)) {
        const auto& tile_layer =
          std::get<render::TiledMap::TileLayer>(layer.data_);

        level->map_->iterate_tile_layer(
          tile_layer,
          [&](auto position, auto index) { spawn_crate(glm::vec2(position)); });
      }
    }
  }

  hud_manager_.get_texts().clear();
  hud_manager_.get_texts().create_named(
    "status", HUDManager::Text{ "", "", 10, glm::vec2(0.5, 0.5), true });

  hud_manager_.get_texts()
    .get_or_create_default("status")
    .set_text("Okay, let's go!")
    .set_fade_effect(HUDManager::Text::FadingEffect(2));
}

auto
GameController::handle(const event::DeleteEntity& event) -> void
{
  if (not world_.has_entity(event.actor_)) {
    return;
  }
  auto& entity = world_.get_entity(event.actor_);
  entity.set_flags(Entity::Flags::marked_for_destruction);
}

auto
GameController::handle(const event::PlayerMoved& event) -> void
{
  if (world_.has_entity(event.actor_)) {
    auto& player = world_.get_entity(event.actor_);

    if (player.flags_.test(Entity::Flags::frozen)) {
      return;
    }

    auto update_visual = [&](unsigned int movement_category) {
      if (event.should_accelerate) {
        player.set_animation(movement_category);
      } else if (not player.controller_.any()) {
        player.set_tile(movement_category);
      }
    };
    switch (event.direction_) {
      case event::PlayerMoved::MoveDirection::left:
        spdlog::trace("Player moved {}, should_accelerate{}",
                      "left",
                      event.should_accelerate);
        player.controller_.moving_left = event.should_accelerate;
        update_visual(3);
        break;
      case event::PlayerMoved::MoveDirection::right:
        spdlog::trace("Player moved {}, should_accelerate{}",
                      "right",
                      event.should_accelerate);
        player.controller_.moving_right = event.should_accelerate;
        update_visual(1);
        break;
      case event::PlayerMoved::MoveDirection::up:
        spdlog::trace("Player moved {}, should_accelerate{}",
                      "up",
                      event.should_accelerate);
        player.controller_.moving_up = event.should_accelerate;
        update_visual(0);
        break;
      case event::PlayerMoved::MoveDirection::down:
        spdlog::trace("Player moved {}, should_accelerate{}",
                      "down",
                      event.should_accelerate);
        player.controller_.moving_down = event.should_accelerate;
        update_visual(2);
        break;
    }
  } else {
    spdlog::debug("Missing player entity ingame");
  }
}

auto
GameController::handle(const event::PlayerDied& event) -> void
{
  using namespace std::chrono_literals;
  if (const auto player_id = world_.get_player_id()) {
    auto& player = world_.get_entity(player_id.value());
    player.set_flags(Entity::Flags::frozen);
    player.set_animation(1);
    spawn_particle(player.aabb_.origin_, 500ms).set_size(glm::vec2(1.5f));
    event_distributor_.enqueue_event(event::DeleteEntity{ player_id.value() },
                                     500ms);
  }

  hud_manager_.get_texts()
    .get_or_create_default("status")
    .set_text("Game over, bastard!")
    .set_fade_effect(HUDManager::Text::FadingEffect{ 2 })
    .set_wave_effect(HUDManager::Text::WaveEffect{ 0, 3 });

  event_distributor_.enqueue_event(event::GameStarted{}, 2000ms);
}

auto
GameController::handle(const event::ParticleDestroyed& event) -> void
{
  if (not entity_exist(event.actor_)) {
    return;
  }

  auto& crate = world_.get_entity(event.actor_);
  crate.set_flags(Entity::Flags::marked_for_destruction);
  spdlog::trace("particle {} destroyed", event.actor_);
}

auto
GameController::handle(const event::CrateDestroyed& event) -> void
{
  if (not entity_exist(event.actor_)) {
    return;
  }

  auto& crate = world_.get_entity(event.actor_);
  crate.set_flags(Entity::Flags::marked_for_destruction);
  spdlog::trace("Crate {} destroyed", event.actor_);

  std::uniform_real_distribution<> pickup_type_distribution(0, 1);
  const auto probability = pickup_type_distribution(random_generator_);
  if (probability > 0.8) {
    spawn_pickup(crate.aabb_.origin_, compute_random_pickup_type());
  }
}

auto
GameController::handle(const event::BombExploded& event) -> void
{
  using namespace bm::game_logic;

  if (not world_.has_entity(event.actor_)) {
    return;
  }

  auto& bomb = world_.get_entity(event.actor_);
  const auto& bomb_data = std::get<BombData>(bomb.data_);

  if (bomb_data.parent_entity_id_ and
      world_.has_entity(*bomb_data.parent_entity_id_)) {
    auto& player = world_.get_entity(*bomb_data.parent_entity_id_);
    auto& player_data = std::get<PlayerData>(player.data_);
    player_data.available_bomb_count_++;
  }

  bomb.flags_.set(Entity::Flags::marked_for_destruction);

  std::default_random_engine generator;
  std::normal_distribution<float> distribution(
    constants::fire_effect_duration_mean_ms,
    constants::fire_effect_duration_sigma_ms);

  const auto spawn_range = bomb_data.prototype_.range_;
  const std::vector<std::pair<glm::ivec2, float>> directions = {
    { glm::ivec2{ 0, 0 }, 1 },
    { glm::ivec2{ 0, 1 }, spawn_range },
    { glm::ivec2{ 0, -1 }, spawn_range },
    { glm::ivec2{ 1, 0 }, spawn_range },
    { glm::ivec2{ -1, 0 }, spawn_range }
  };

  const auto initial_origin =
    glm::ivec2(bomb.aabb_.origin_.x, bomb.aabb_.origin_.y);

  for (const auto [direction, range] : directions) {
    for (size_t i = 1; i <= range; i++) {
      const auto pose = initial_origin + direction * glm::ivec2(i);
      if (world_.has_static_collision(pose)) {
        break;
      }

      bool affects_dynamic_object = world_.is_cell_occupied(pose);

      using namespace std::chrono_literals;
      const auto duration =
        1000ms +
        std::chrono::milliseconds{ static_cast<int>(distribution(generator)) };

      spawn_temporary_fire(pose, duration);
      if (affects_dynamic_object) {
        break;
      }
    }
  }

  spdlog::trace("Bomb exploded with range: {}", spawn_range);

  hud_manager_.get_texts()
    .get_or_create_default("status")
    .set_text("Bomb exploded!")
    .set_fade_effect(HUDManager::Text::FadingEffect{ 1 })
    .set_wave_effect(HUDManager::Text::WaveEffect{ 0, 5 });
}

auto
GameController::handle(const event::FireTerminated& event) -> void
{
  if (not world_.has_entity(event.actor_)) {
    return;
  }

  auto& fire = world_.get_entity(event.actor_);
  fire.flags_.set(Entity::marked_for_destruction);
}

auto
GameController::handle(const event::PlayerActionEvent& event) -> void
{
  using namespace bm::game_logic;

  if (const auto player_id = world_.get_player_id()) {
    auto& player = world_.get_entity(*player_id);
    auto& player_data = std::get<PlayerData>(player.data_);

    switch (player_data.weapon_) {

      case WeaponType::bomb: {

        if (not player_data.available_bomb_count_) {
          spdlog::trace("Player has zero available bombs, skipping plant");
          return;
        }

        const auto coords = glm::round(player.aabb_.origin_);
        spawn_bomb(coords, player_data.bomb_prototype_, player_id);
        player_data.available_bomb_count_--;

        hud_manager_.get_texts()
          .get_or_create_default("status")
          .set_text("Bomb planted!")
          .set_fade_effect(HUDManager::Text::FadingEffect{ 1 })
          .set_wave_effect(HUDManager::Text::WaveEffect{ 0, 5 });
      } break;
      case WeaponType::immediate_fire: {
        spdlog::trace("immediate_fire not implemented");
      } break;
      case WeaponType::flood_bomb: {
        spdlog::trace("flood_bomb not implemented");
      } break;
    }
  }
}

auto
GameController::handle(const event::PickedPickupItem& event) -> void
{
  if (not world_.has_entity(event.pickup_id) or
      not world_.has_entity(event.player_id)) {
    return;
  }

  auto& pickup = world_.get_entity(event.pickup_id);
  pickup.flags_.set(Entity::Flags::marked_for_destruction);

  auto& player = world_.get_entity(event.player_id);

  using namespace bm::game_logic;

  if (not std::holds_alternative<PickupData>(pickup.data_)) {
    spdlog::warn("Pickup '{}' is not holding pickup data", event.pickup_id);
    return;
  }

  if (not std::holds_alternative<PlayerData>(player.data_)) {
    spdlog::warn("Player '{}' is not holding player data", event.player_id);
    return;
  }

  const auto& pickup_data = std::get<PickupData>(pickup.data_);
  auto& player_data = std::get<PlayerData>(player.data_);
  switch (pickup_data.type_) {
    case PickupType::increase_bomb_count:
      player_data.available_bomb_count_++;
      break;
    case PickupType::increase_bomb_range:
      player_data.bomb_prototype_.range_ += 1;
      break;

    default:
      spdlog::warn(
        "Pickup '{}' not used, behavior not defined for pickup type '{}'",
        static_cast<unsigned>(pickup_data.type_));
  }
}

auto
GameController::handle(const event::EntityCollide& event) -> void
{
  const auto collision_response =
    std::unordered_map<std::pair<Entity::Type, Entity::Type>,
                       std::function<void(Entity&, Entity&)>,
                       pair_hash>{
      { { Entity::Type::pickup, Entity::Type::player },
        [&](Entity& pickup, Entity& player) {
          event_distributor_.enqueue_event(
            event::PickedPickupItem{ pickup.get_id(), player.get_id() });

          spdlog::debug("Picked up a pickup");
        } },
      { { Entity::Type::fire, Entity::Type::player },
        [&](Entity& fire, Entity& player) {
          event_distributor_.enqueue_event(event::PlayerDied{});
        } },
      { { Entity::Type::fire, Entity::Type::crate },
        [&](Entity& fire, Entity& crate) {
          event_distributor_.enqueue_event(
            event::CrateDestroyed{ crate.get_id() });
        } }
    };

  if (not entity_exist(event.actor_a_, event.actor_b_)) {
    return;
  }

  auto& entity_a = world_.get_entity(event.actor_a_);
  auto& entity_b = world_.get_entity(event.actor_b_);

  const auto collision_type = std::make_pair(entity_a.type_, entity_b.type_);
  const auto collision_type_reversed =
    std::make_pair(entity_b.type_, entity_a.type_);
  if (collision_response.count(collision_type)) {
    collision_response.at(collision_type)(entity_a, entity_b);
  } else if (collision_response.count(collision_type_reversed)) {
    collision_response.at(collision_type_reversed)(entity_b, entity_a);
  } else {
    return;
  }
}

auto
GameController::spawn_bomb(glm::vec2 position,
                           game_logic::BombPrototype type,
                           std::optional<Entity::Id> parent) -> Entity&
{
  auto& entity = world_.create(Entity::Type::bomb)
                   .set_tile(12)
                   .set_data(bm::game_logic::BombData{ *parent, type })
                   .set_origin(position);

  using namespace std::chrono_literals;
  event_distributor_.enqueue_event(event::BombExploded{ entity.get_id() },
                                   2000ms);

  spdlog::trace("spawn_bomb at ({}, {})", position.x, position.y);
  return entity;
}

auto
GameController::spawn_crate(glm::vec2 position) -> Entity&
{
  auto& entity = world_.create(Entity::Type::crate)
                   .set_tile(0)
                   .set_tileset("crate.json")
                   .set_origin(position)
                   .set_flags(Entity::Flags::frozen);
  spdlog::trace("spawn_crate: pose ({}, {})", position.x, position.y);
  return entity;
}

auto
GameController::spawn_pickup(glm::vec2 position, game_logic::PickupType type)
  -> Entity&
{
  std::unordered_map<game_logic::PickupType, unsigned> type_to_tile = {
    { game_logic::PickupType::increase_bomb_count, 0 },
    { game_logic::PickupType::increase_bomb_range, 1 },
    { game_logic::PickupType::freeze_for_some_time, 2 },
  };
  if (type_to_tile.count(type) == 0) {
    throw std::runtime_error(
      fmt::format("Missing implementation for pickup type {}", type));
  }

  auto& pickup = world_.create(Entity::Type::pickup)
                   .set_tile(type_to_tile.at(type))
                   .set_tileset("potions.json")
                   .set_origin(position)
                   .set_data(game_logic::PickupData{ type });
  spdlog::trace("spawn_pickup: pose ({}, {}), type: {}",
                position.x,
                position.y,
                static_cast<unsigned>(type));
  return pickup;
}

auto
GameController::spawn_temporary_fire(glm::vec2 position,
                                     std::chrono::milliseconds duration)
  -> Entity&
{
  const auto fire_id = world_.create(Entity::Type::fire)
                         .set_tileset("fire.json")
                         .set_animation(0)
                         .set_origin(position)
                         .get_id();

  event_distributor_.enqueue_event(event::FireTerminated{ fire_id }, duration);

  spdlog::trace("spawn_temporary_fire: pose ({}, {}), duration: {}",
                position.x,
                position.y,
                duration.count());
  return world_.get_entity(fire_id);
}

auto
GameController::spawn_particle(glm::vec2 position,
                               std::chrono::milliseconds duration,
                               const std::string tileset,
                               unsigned animation_id) -> Entity&
{
  auto& particle = world_.create(Entity::Type::particle)
                     .set_tileset(tileset)
                     .set_animation(animation_id)
                     .set_origin(position);

  event_distributor_.enqueue_event(
    event::ParticleDestroyed{ particle.get_id() }, duration);

  spdlog::trace("spawn_particle: pose ({}, {}), duration: {}",
                position.x,
                position.y,
                duration.count());
  return particle;
}

auto
GameController::compute_random_pickup_type() -> bm::game_logic::PickupType
{
  std::uniform_int_distribution<> pickup_type_distribution(
    0, bm::game_logic::PickupType::last - 1);

  return static_cast<bm::game_logic::PickupType>(
    pickup_type_distribution(random_generator_));
}

template<typename... Args>
auto
GameController::entity_exist(Entity::Id id, Args... args) -> bool
{
  if constexpr (sizeof...(args) == 0) {
    return world_.has_entity(id);
  } else {
    return entity_exist(args...) && world_.has_entity(id);
  }
}