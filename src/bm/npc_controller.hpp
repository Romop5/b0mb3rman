#pragma once

#include <random>

#include <bm/navigation_mesh.hpp>
#include <bm/world.hpp>

namespace bm {

class NPCController
{
public:
  NPCController(EventDistributor& event_distributor, bm::World& world);

  auto update() -> void;

private:
  auto update_random_movement(bm::Entity& entity) -> void;
  auto update_chasing_target(bm::Entity& entity) -> void;

private:
  EventDistributor& event_distributor_;
  bm::World& world_;
  NavigationMesh navigation_mesh_;

  std::default_random_engine generator;
};
} // namespace bm