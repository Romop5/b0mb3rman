#pragma once

#include <bm/entity.hpp>
#include <vector>

namespace bm {
struct World
{
  std::vector<std::unique_ptr<Entity>> entities_;
};

} // namespace bm
