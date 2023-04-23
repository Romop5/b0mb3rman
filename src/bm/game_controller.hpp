#pragma once

#include <chrono>
#include <queue>
#include <random>
#include <variant>
#include <vector>

#include <bm/entity.hpp>
#include <bm/event_distributor.hpp>
#include <bm/events.hpp>
#include <bm/game_logic.hpp>
#include <bm/hud_manager.hpp>
#include <bm/world.hpp>

namespace bm {

/**
 * @brief Event-based game logic processing
 */
class GameController
{

public:
  GameController(EventDistributor& event_distributor,
                 HUDManager& hud_manager,
                 World& world)
    : event_distributor_{ event_distributor }
    , hud_manager_{ hud_manager }
    , world_{ world }
  {
    event_distributor_.registry_listener<event::GameStarted,
                                         event::PlayerMoved,
                                         event::PlayerDied,
                                         event::BombExploded,
                                         event::BombPlanted,
                                         event::EntityCollide,
                                         event::FireTerminated,
                                         event::PickedPickupItem>(*this);
  }

public:
  auto handle(const event::GameStarted& event) -> void;
  auto handle(const event::PlayerMoved& event) -> void;
  auto handle(const event::PlayerDied& event) -> void;
  auto handle(const event::BombExploded& event) -> void;
  auto handle(const event::FireTerminated& event) -> void;
  auto handle(const event::BombPlanted& event) -> void;
  auto handle(const event::PickedPickupItem& event) -> void;
  auto handle(const event::EntityCollide& event) -> void;

protected:
  auto spawn_temporary_fire(glm::vec2 position,
                            std::chrono::milliseconds duration) -> Entity::Id;

private:
  template<typename... Args>
  auto entity_exist(Entity::Id id, Args... args) -> bool;

private:
  EventDistributor& event_distributor_;

  HUDManager& hud_manager_;
  World& world_;
};
} // namespace bm