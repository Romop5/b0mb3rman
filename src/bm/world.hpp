#pragma once

#include <vector>

#include <bm/entity.hpp>

namespace bm {
struct World
{
  std::vector<std::unique_ptr<Entity>> entities_;
};

} // namespace bm