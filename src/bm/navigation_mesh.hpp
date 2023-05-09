#pragma once

#include <bm/world.hpp>

namespace bm {

class NavigationMesh
{
public:
  NavigationMesh(bm::interfaces::ICollisionWorld& world);

  auto update() -> void;

  auto is_reachable(glm::vec2 start, glm::vec2 end) -> bool;
  auto compute_path(glm::vec2 start, glm::vec2 end) -> std::vector<glm::vec2>;

private:
  bm::interfaces::ICollisionWorld& world_;
};
} // namespace bm