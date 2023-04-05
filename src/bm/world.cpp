#include <bm/events.hpp>
#include <bm/world.hpp>

using namespace bm;

namespace {

} // namespace

World::World(EventDistributor& event_distributor)
  : event_distributor_{ event_distributor }
{
}

auto
World::get_player_id() -> std::optional<Entity::Id>
{
  for (auto& [id, entity] : entities_) {
    if (entity.type_ == Entity::Type::player) {
      return id;
    }
  }
  return {};
}

auto
World::get_entity(Entity::Id id) -> Entity&
{
  return entities_.at(id);
}

auto
World::has_entity(Entity::Id id) const -> bool
{
  return entities_.count(id);
}

auto
World::clear() -> void
{
  entities_.clear();
}

auto
World::delete_marked_entities() -> void
{
  const auto predicate = [](const Entity& entity) {
    return entity.flags_.test(Entity::Flags::marked_for_destruction);
  };

  // TODO: C++20 provides erase_if
  for (auto i = entities_.begin(), last = entities_.end(); i != last;) {
    if (predicate(i->second)) {
      i = entities_.erase(i);
    } else {
      ++i;
    }
  }
}

auto
World::update(std::chrono::milliseconds delta) -> void
{
  const auto elapsed_seconds = delta.count() / 1000.0f;

  /* Update acceleration and speed */
  for (auto& [id, entity] : entities_) {
    const auto& controller = entity.controller_;
    entity.acceleration_ =
      glm::vec2(controller.moving_right) * glm::vec2(1.0, 0.0) +
      glm::vec2(controller.moving_left) * glm::vec2(-1.0, 0.0) +
      glm::vec2(controller.moving_down) * glm::vec2(0.0, 1.0) +
      glm::vec2(controller.moving_up) * glm::vec2(0.0, -1.0);

    entity.velocity_ += entity.acceleration_;

    const auto current_speed = glm::length(entity.velocity_);
    if (current_speed < 0.001) {
      continue;
    }

    const auto new_speed =
      glm::min(entity.max_speed_, glm::length(entity.velocity_));

    const auto speed_adjustment = new_speed / current_speed;
    entity.velocity_ *= speed_adjustment;
  }

  /* Update positions */
  for (auto& [id, entity] : entities_) {
    const auto position_delta = elapsed_seconds * entity.velocity_;
    entity.aabb_.origin_ += position_delta;

    if (glm::length(position_delta) > 0.01) {
      spdlog::trace("Updated entity {} to position ({},{}) (delta: {},{})",
                    id,
                    entity.aabb_.origin_.x,
                    entity.aabb_.origin_.y,
                    position_delta.x,
                    position_delta.y);
    }
  }

  /* Decrease speed (attenuation) */
  const auto attenuation = 10.0f;
  for (auto& [id, entity] : entities_) {
    entity.velocity_ +=
      entity.velocity_ *
      glm::vec2(-std::min(elapsed_seconds * attenuation, 1.0f));
  }

  detect_collisions();
}

auto
World::detect_collisions() -> void
{
  // naive O(N^2) comparison
  for (const auto& [id_a, entity_a] : entities_) {
    for (const auto& [id_b, entity_b] : entities_) {
      if (id_b <= id_a)
        continue;

      // TODO: introduce collision groups & masks
      if (entity_a.type_ == entity_b.type_) {
        continue;
      }

      if (entity_a.flags_.test(Entity::Flags::marked_for_destruction) ||
          entity_b.flags_.test(Entity::Flags::marked_for_destruction)) {
        continue;
      }

      const auto& a = entity_a.aabb_;
      const auto& b = entity_b.aabb_;

      if (a.collide(b)) {
        spdlog::debug("Entity {} and {} collide", id_a, id_b);
        event_distributor_.enqueue_event(
          bm::event::EntityCollide{ id_a, id_b });
      }
    }
  }
}
