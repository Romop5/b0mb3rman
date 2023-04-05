#pragma once

#include <bm/entity.hpp>

namespace bm {

namespace event {
namespace meta {
struct EntityUpdate
{
  Entity::Id actor_;
};
} // namespace bm::events::meta

/* Game-related events*/
struct GameStarted
{};

/* Entity-related events*/
struct PlayerMoved
{
  bool should_accelerate;
  enum class MoveDirection
  {
    up,
    down,
    left,
    right
  } direction_;
};

struct PlayerDied : public meta::EntityUpdate
{};

struct BombExploded : public meta::EntityUpdate
{};
struct BombPlanted
{};

struct EntityCollide
{
  Entity::Id actor_a_;
  Entity::Id actor_b_;
};

} // namespace bm::events
} // namespace bm