#pragma once

#include <bm/level.hpp>

namespace bm {
namespace interfaces {
/**
 * @brief Abstract representation of game API, accessible by script logic
 *
 */
class IGame
{
public:
  virtual auto get_current_level() const -> const bm::Level* = 0;
};
} // namespace bm::interfaces
} // namespace bm