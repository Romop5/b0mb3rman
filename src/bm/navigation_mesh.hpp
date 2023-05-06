#pragma once

#include <bm/world.hpp>

namespace bm {

class NavigationMesh
{
public:
  NavigationMesh(bm::interfaces::ICollisionWorld& world);

  auto update() -> void;

private:
  bm::interfaces::ICollisionWorld& world_;
};
} // namespace bm