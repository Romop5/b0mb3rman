#pragma once

#include <bm/world.hpp>

namespace bm {

class NPCController
{
public:
  NPCController(EventDistributor& event_distributor, bm::World& world);

  auto update() -> void;

private:
  EventDistributor& event_distributor_;
  bm::World& world_;
};
} // namespace bm