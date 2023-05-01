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
#include <bm/interfaces/game.hpp>
#include <bm/world.hpp>

namespace bm {

/**
 * @brief Event-based game logic processing
 */
class GameController
{

public:
  GameController(bm::interfaces::IGame& game,
                 EventDistributor& event_distributor,
                 HUDManager& hud_manager,
                 World& world)
    : game_{ game }
    , event_distributor_{ event_distributor }
    , hud_manager_{ hud_manager }
    , world_{ world }
  {
    event_distributor_.registry_listener<event::GameStarted,
                                         event::DeleteEntity,
                                         event::PlayerMoved,
                                         event::PlayerDied,
                                         event::ParticleDestroyed,
                                         event::CrateDestroyed,
                                         event::BombExploded,
                                         event::PlayerActionEvent,
                                         event::EntityCollide,
                                         event::FireTerminated,
                                         event::PickedPickupItem>(*this);
  }

public:
  auto handle(const event::GameStarted& event) -> void;
  auto handle(const event::DeleteEntity& event) -> void;
  auto handle(const event::PlayerMoved& event) -> void;
  auto handle(const event::PlayerDied& event) -> void;
  auto handle(const event::ParticleDestroyed& event) -> void;
  auto handle(const event::CrateDestroyed& event) -> void;
  auto handle(const event::BombExploded& event) -> void;
  auto handle(const event::FireTerminated& event) -> void;
  auto handle(const event::PlayerActionEvent& event) -> void;
  auto handle(const event::PickedPickupItem& event) -> void;
  auto handle(const event::EntityCollide& event) -> void;

protected:
  auto spawn_bomb(glm::vec2 position,
                  game_logic::BombPrototype type = {},
                  std::optional<Entity::Id> parent = std::nullopt) -> Entity&;
  auto spawn_crate(glm::vec2 position) -> Entity&;
  auto spawn_pickup(glm::vec2 position, game_logic::PickupType type) -> Entity&;

  auto spawn_temporary_fire(glm::vec2 position,
                            std::chrono::milliseconds duration) -> Entity&;

  auto spawn_particle(glm::vec2 position,
                      std::chrono::milliseconds duration,
                      const std::string tileset = "fire.json",
                      unsigned animation_id = 0) -> Entity&;

  auto compute_random_pickup_type() -> bm::game_logic::PickupType;

private:
  template<typename... Args>
  auto entity_exist(Entity::Id id, Args... args) -> bool;

private:
  bm::interfaces::IGame& game_;
  EventDistributor& event_distributor_;

  HUDManager& hud_manager_;
  World& world_;

  std::mt19937 random_generator_;
};
} // namespace bm