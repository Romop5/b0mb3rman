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

    /*if (npc_data.ticks_to_change) {
      npc_data.ticks_to_change--;
      continue;
    }
    npc_data.ticks_to_change = 10;
*/
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
  // animation movement pending
  if (entity.controller_.animation_next_position) {
    return;
  }

  auto& npc_data = std::get<game_logic::NPCData>(entity.data_);

  if (not world_.has_entity(npc_data.target_id)) {
    return;
  }

  auto& target = world_.get_entity(npc_data.target_id);

  auto& path = npc_data.trajectory;
  if (path.empty()) {
    try {
      path = navigation_mesh_.compute_path(entity.aabb_.origin_,
                                           target.aabb_.origin_);
      // path.push_back(target.aabb_.origin_);
    } catch (std::exception& e) {
      path = {};
    }
  }
  if (path.empty()) {
    return;
  }

  auto next_position = path.at(0);
  entity.controller_.animation_next_position = next_position;
  path.erase(path.begin());

  /*auto direction = [this, epsilon](glm::vec2 difference)
    -> std::optional<bm::event::PlayerMoved::MoveDirection> {
    const auto absolute_delta = glm::abs(difference);

    if (absolute_delta.x > epsilon) {
      return difference.x > 0 ? bm::event::PlayerMoved::MoveDirection::right
                              : bm::event::PlayerMoved::MoveDirection::left;
    } else if (absolute_delta.y > epsilon) {
      return difference.y > 0 ? bm::event::PlayerMoved::MoveDirection::down
                              : bm::event::PlayerMoved::MoveDirection::up;
    }
    return {};
  }(difference);

  if (direction) {
    event_distributor_.enqueue_event(
      bm::event::PlayerMoved{ entity.get_id(), true, direction.value() });
  } else {
    event_distributor_.enqueue_event(bm::event::PlayerMoved{
      entity.get_id(), false, bm::event::PlayerMoved::MoveDirection::down });
  }
  */
}