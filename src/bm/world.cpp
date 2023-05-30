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

    if (entity.flags_.test(Entity::Flags::animated_movement)) {
      continue;
    }

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
    if (entity.flags_.test(Entity::Flags::frozen) or
        entity.flags_.test(Entity::Flags::animated_movement)) {
      continue;
    }

    const auto position_delta = elapsed_seconds * entity.velocity_;
    if (glm::length(position_delta) < 1e-4) {
      continue;
    }

    /*const auto new_position_x =
      entity.aabb_.origin_ + glm::vec2(position_delta.x, 0);
    const auto new_position_y =
      entity.aabb_.origin_ + glm::vec2(0, position_delta.y);
    const auto new_position_xy = entity.aabb_.origin_ + position_delta;
    */

    /*auto generate_potential_collision_ids(glm::vec2 pos)
      ->std::array<4, glm::vec2>
    {
    }*/

    std::array<glm::vec2, 4> test_cells_during_movement{
      glm::vec2(0, 0),
      glm::vec2(entity.aabb_.size_.x, 0),
      glm::vec2(0, entity.aabb_.size_.y),
      glm::vec2(entity.aabb_.size_),
    };

    glm::vec2 update_delta = position_delta;
    {
      // test move left or right
      const auto new_position =
        entity.aabb_.origin_ + glm::vec2(position_delta.x, 0);

      for (const auto& cell : test_cells_during_movement) {
        const auto cell_position = new_position + cell;
        if (is_point_colliding(cell_position, entity.collision_mask_)) {
          update_delta.x = 0;
          break;
        }
      }
    }
    {
      // test move left or right
      const auto new_position =
        entity.aabb_.origin_ + glm::vec2(0, position_delta.y);

      for (const auto& cell : test_cells_during_movement) {
        const auto cell_position = new_position + cell;
        if (is_point_colliding(cell_position, entity.collision_mask_)) {
          update_delta.y = 0;
          break;
        }
      }
    }

    entity.aabb_.origin_ += update_delta;

    if (not entity.flags_.test(Entity::Flags::unbounded)) {
      entity.aabb_.put_inside(boundary_);
    }

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

  /* Animated-only: update position (constant speed movement)*/
  for (auto& [id, entity] : entities_) {
    auto& controller = entity.controller_;

    if (not entity.flags_.test(Entity::Flags::animated_movement) or
        not controller.animation_next_position) {
      continue;
    }

    const auto next_position = *controller.animation_next_position;
    const auto remaining_difference = next_position - entity.aabb_.origin_;
    if (glm::length(remaining_difference) < 1e-3) {
      entity.aabb_.origin_ = next_position;
      controller.animation_next_position.reset();
      continue;
    }
    const auto direction = glm::normalize(remaining_difference);

    const auto velocity = entity.max_speed_ * direction;
    const auto position_delta = elapsed_seconds * velocity;

    // missed, clamped to next_position
    if (glm::length(position_delta) > glm::length(remaining_difference)) {
      entity.aabb_.origin_ = next_position;
      controller.animation_next_position.reset();
    } else {
      entity.aabb_.origin_ += position_delta;
    }
  }
  detect_collisions();
}
auto
World::update_boundary(glm::vec2 top_left, glm::vec2 bottom_right) -> void
{
  const auto origin = top_left;
  const auto size = bottom_right - top_left;
  boundary_ = utils::AABB{ origin, size };
}

auto
World::update_static_collisions(utils::OccupancyMap2D<bool> map) -> void
{
  static_collisions_ = std::move(map);
}

auto
World::get_world_boundaries() -> utils::AABB
{
  return boundary_;
}

auto
World::is_out_of_bounds(glm::vec2 position) -> bool
{
  return !boundary_.contains(position);
}

auto
World::is_point_colliding(glm::vec2 position, Entity::TypeMask allowed_types)
  -> bool
{
  if (has_static_collision(position)) {
    return true;
  }

  for (const auto& [id, entity] : entities_) {
    if (not entity.flags_.test(Entity::Flags::frozen) or
        not allowed_types.test(entity.get_type()))
      continue;
    if (entity.aabb_.contains(position))
      return true;
  }
  return false;
}

auto
World::is_cell_occupied(glm::vec2 position, Entity::TypeMask allowed_types)
  -> bool
{
  if (has_static_collision(position)) {
    return true;
  }

  utils::AABB cell_aabb{ position, glm::vec2(1.0f, 1.0f) };
  for (const auto& [id, entity] : entities_) {
    if (not entity.flags_.test(Entity::Flags::frozen) or
        not allowed_types.test(entity.get_type()))
      continue;
    if (entity.aabb_.collide(cell_aabb))
      return true;
  }
  return false;
}

auto
World::has_static_collision(glm::vec2 pos) -> bool
{
  auto floor_pos = glm::floor(pos);
  if (is_out_of_bounds(floor_pos)) {
    return false;
  }
  try {
    return static_collisions_.at({ static_cast<unsigned int>(floor_pos.x),
                                   static_cast<unsigned int>(floor_pos.y) });
  } catch (std::exception& e) {
    spdlog::error("has_static_collision: unexpected exception: {}", e.what());
    return false;
  }
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