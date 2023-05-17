#include <bm/entity.hpp>
#include <bm/events.hpp>
#include <bm/npc_controller.hpp>

using namespace bm;

namespace {
} // namespace

NPCController::NPCController(EventDistributor& event_distributor,
                             bm::World& world)
  : event_distributor_{ event_distributor }
  , world_{ world }
  , navigation_mesh_{ world }
{
}

auto
NPCController::update() -> void
{
  navigation_mesh_.update();

  for (auto& [id, entity] : world_) {
    if (entity.get_type() != Entity::Type::npc) {
      continue;
    }
    if (not std::holds_alternative<game_logic::NPCData>(entity.data_)) {
      spdlog::warn("Entity '{}' is NPC, but does not have NPCData", id);
      continue;
    }

    auto& npc_data = std::get<game_logic::NPCData>(entity.data_);

    if (npc_data.ticks_to_change) {
      npc_data.ticks_to_change--;
      continue;
    }
    npc_data.ticks_to_change = 1;

    spdlog::trace("update(): entity '{}'", id);

    using namespace bm::game_logic;
    switch (npc_data.goal) {
      case NPCState::random_walk: {
        update_random_movement(entity);
      } break;

      case NPCState::chasing_target: {
        update_chasing_target(entity);
      } break;
    }
  }
}

auto
NPCController::update_random_movement(bm::Entity& entity) -> void
{
  entity.controller_ = {};
  const auto directions_count =
    static_cast<unsigned>(bm::event::PlayerMoved::MoveDirection::count);

  std::uniform_int_distribution<> distribution(0, directions_count - 1);
  const auto direction_id = distribution(generator);
  const auto direction_enum =
    static_cast<bm::event::PlayerMoved::MoveDirection>(direction_id);

  event_distributor_.enqueue_event(
    bm::event::PlayerMoved{ entity.get_id(), true, direction_enum });
}

auto
NPCController::update_chasing_target(bm::Entity& entity) -> void
{
  entity.controller_ = {};

  auto& npc_data = std::get<game_logic::NPCData>(entity.data_);

  if (not world_.has_entity(npc_data.target_id)) {
    return;
  }

  auto& target = world_.get_entity(npc_data.target_id);

  const auto path =
    navigation_mesh_.compute_path(entity.aabb_.origin_, target.aabb_.origin_);

  if (path.empty() or path.size() == 1) {
    return;
  }

  const auto current_position = entity.aabb_.origin_;
  const auto next_position = path.at(1);
  const auto difference = next_position - current_position;

  auto direction = [this](glm::vec2 difference) {
    if (abs(difference.x) > 1e-6) {
      return difference.x > 0 ? bm::event::PlayerMoved::MoveDirection::right
                              : bm::event::PlayerMoved::MoveDirection::left;
    } else if (abs(difference.y) > 1e-6) {
      return difference.y > 0 ? bm::event::PlayerMoved::MoveDirection::down
                              : bm::event::PlayerMoved::MoveDirection::up;
    }
  }(difference);

  event_distributor_.enqueue_event(
    bm::event::PlayerMoved{ entity.get_id(), true, direction });
}