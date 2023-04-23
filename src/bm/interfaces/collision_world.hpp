#pragma once

namespace bm {
namespace interfaces {
/**
 * @brief Abstract representation of a collision engine API
 *
 */
class ICollisionWorld
{
public:
  virtual auto is_out_of_bounds(unsigned int x, unsigned int y) -> bool = 0;
  virtual auto is_cell_occupied(unsigned int x, unsigned int y) -> bool = 0;
  virtual auto has_static_collision(unsigned int x, unsigned int y) -> bool = 0;
};
} // namespace bm::interfaces
} // namespace bm