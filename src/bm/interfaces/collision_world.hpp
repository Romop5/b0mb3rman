#pragma once

#include <bm/entity.hpp>
#include <glm/glm.hpp>

namespace bm {
namespace interfaces {
/**
 * @brief Abstract representation of a collision engine API
 *
 */
class ICollisionWorld
{
public:
  virtual auto get_world_boundaries() -> utils::AABB = 0;
  virtual auto is_out_of_bounds(glm::vec2 position) -> bool = 0;
  virtual auto is_cell_occupied(glm::vec2 position,
                                Entity::TypeMask allowed_types) -> bool = 0;
  virtual auto has_static_collision(glm::vec2 position) -> bool = 0;
};
} // namespace bm::interfaces
} // namespace bm