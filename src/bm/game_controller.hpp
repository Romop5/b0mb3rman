#pragma once

#include <chrono>
#include <queue>
#include <variant>
#include <vector>

#include <bm/entity.hpp>
#include <bm/event_distributor.hpp>

namespace bm {

namespace event {
namespace meta {
struct EntityUpdate
{
  Entity::Id actor_;
};
} // namespace bm::events::meta

struct PlayerDied : public meta::EntityUpdate
{};
struct BombExploded : public meta::EntityUpdate
{};
struct BombPlanted : public meta::EntityUpdate
{};

} // namespace bm::events

/**
 * @brief Event queue with associated processing logic
 *
 * @note Discrete time step simulator with strongly-typed events
 */
class GameController
{

public:
  GameController(EventDistributor& event_distributor)
    : event_distributor_{ event_distributor }
  {
    event_distributor_.registry_listener<event::PlayerDied>(*this);
  }

public:
  auto handle(const event::PlayerDied& event) -> void {}

  auto handle(const event::BombExploded& event) -> void {}

  auto handle(const event::BombPlanted& event) -> void {}

private:
  EventDistributor& event_distributor_;
};
} // namespace bm