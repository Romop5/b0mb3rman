#pragma once

#include <chrono>
#include <optional>
#include <vector>

#include <bm/entity.hpp>
#include <bm/event_distributor.hpp>
#include <bm/interfaces/collision_world.hpp>
#include <utils/aabb.hpp>
#include <utils/occupancy_map.hpp>

namespace bm {
class World : public bm::interfaces::ICollisionWorld
{
public:
  explicit World(EventDistributor& event_distributor);

  template<typename... Args>
  auto create(Args... args) -> Entity&
  {
    auto id = next_id_++;
    entities_.emplace(id, std::move(Entity{ id, args... }));
    return get_entity(id);
  }

  auto begin() { return entities_.begin(); }
  auto end() { return entities_.end(); }

  auto get_player_id() -> std::optional<Entity::Id>;
  auto get_entity(Entity::Id id) -> Entity&;
  auto has_entity(Entity::Id id) const -> bool;
  auto clear() -> void;

  auto delete_marked_entities() -> void;
  auto update(std::chrono::milliseconds delta) -> void;

  auto update_boundary(glm::vec2 top_left, glm::vec2 bottom_right) -> void;
  auto update_static_collisions(utils::OccupancyMap2D<bool> map) -> void;

  /* ICollisionWorld */
  auto is_out_of_bounds(unsigned int x, unsigned int y) -> bool override final;
  auto is_cell_occupied(unsigned int x, unsigned int y) -> bool override final;
  auto has_static_collision(unsigned int x, unsigned int y)
    -> bool override final;
  auto has_static_collision(glm::vec2 position) -> bool;

protected:
  auto detect_collisions() -> void;

private:
  EventDistributor& event_distributor_;

  Entity::Id next_id_{ 0 };
  std::unordered_map<Entity::Id, Entity> entities_;

  utils::AABB boundary_;
  utils::OccupancyMap2D<bool> static_collisions_;
};

} // namespace bm
