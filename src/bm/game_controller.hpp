#pragma once

#include <chrono>
#include <queue>
#include <variant>
#include <vector>

#include <bm/entity.hpp>
#include <bm/event_distributor.hpp>
#include <bm/events.hpp>
#include <bm/world.hpp>

namespace bm {

/**
 * @brief Event-based game logic processing
 */
class GameController
{

public:
  GameController(EventDistributor& event_distributor, World& world)
    : event_distributor_{ event_distributor }
    , world_{ world }
  {
    event_distributor_.registry_listener<event::GameStarted,
                                         event::PlayerMoved,
                                         event::PlayerDied,
                                         event::BombExploded,
                                         event::BombPlanted,
                                         event::EntityCollide>(*this);
  }

public:
  auto handle(const event::GameStarted& event) -> void
  {
    world_.clear();
    world_.create(Entity::Type::player).set_tile(10);
    world_.create(Entity::Type::pickup).set_tile(8).set_origin({ 5, 5 });
    world_.create(Entity::Type::pickup).set_tile(8).set_origin({ 15, 5 });
    world_.create(Entity::Type::pickup).set_tile(8).set_origin({ 8, 7 });
    world_.create(Entity::Type::pickup).set_tile(8).set_origin({ 3, 11 });
  }

  auto handle(const event::PlayerMoved& event) -> void
  {
    const auto player_id = world_.get_player_id();
    if (player_id) {
      auto& player = world_.get_entity(player_id.value());
      auto& entity_origin = player.aabb_.origin_;
      entity_origin += event.direction_;
    } else {
      spdlog::debug("Missing player entity ingame");
    }
  }

  auto handle(const event::PlayerDied& event) -> void {}

  auto handle(const event::BombExploded& event) -> void {}

  auto handle(const event::BombPlanted& event) -> void
  {
    const auto coords = glm::vec2{ 10, 10 };
    world_.create(Entity::Type::bomb).set_origin(coords).set_tile(12);

    spdlog::trace("Planting bomb at ({}, {})", coords.x, coords.y);
  }

  auto handle(const event::EntityCollide& event) -> void
  {
    if (entity_exist(event.actor_a_, event.actor_b_)) {
      auto& entity_a = world_.get_entity(event.actor_a_);
      auto& entity_b = world_.get_entity(event.actor_b_);

      if (entity_a.type_ == Entity::Type::pickup ||
          entity_b.type_ == Entity::Type::pickup) {
        auto& pickup =
          (entity_a.type_ == Entity::Type::pickup) ? entity_a : entity_b;
        pickup.flags_.set(Entity::Flags::marked_for_destruction);

        spdlog::debug("Picked up a pickup");
      }
    }
  }

protected:
  template<typename... Args>
  auto entity_exist(Entity::Id id, Args... args) -> bool
  {
    if constexpr (sizeof...(args) == 0) {
      return world_.has_entity(id);
    } else {
      return entity_exist(args...) && world_.has_entity(id);
    }
  }

private:
  EventDistributor& event_distributor_;
  World& world_;
};
} // namespace bm