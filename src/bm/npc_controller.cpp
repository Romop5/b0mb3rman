#include <bm/entity.hpp>
#include <bm/npc_controller.hpp>

using namespace bm;

NPCController::NPCController(bm::World& world)
  : event_distributor_{ event_distributor_ }
  , world_{ world }
{
}

auto
NPCController::update() -> void
{
  for (auto& [id, entity] : world_) {
    if (entity.get_type() != Entity::Type::npc) {
      continue;
    }
    if (not std::holds_alternative<game_logic::NPCData>(entity.data_)) {
      continue;
    }

    const auto& npc_data = std::get<game_logic::NPCData>(entity.data_);
    }
}
