#pragma once

#include <vector>

#include <glm/glm.hpp>

namespace bm {

namespace game_logic {

namespace constants {
constexpr auto fire_effect_duration_mean_ms = 200.0f;
constexpr auto fire_effect_duration_sigma_ms = 50.0f;
} // namespace constants

enum PickupType
{
  increase_bomb_count,
  increase_bomb_range,
  freeze_for_some_time,
  last
};

enum WeaponType
{
  bomb,
  flood_bomb,
  immediate_fire
};

struct BombPrototype
{
  unsigned int range_{ 1 };
};

struct PlayerData
{
  WeaponType weapon_{ WeaponType::bomb };
  BombPrototype bomb_prototype_;
  unsigned int available_bomb_count_{ 1 };
};

struct PickupData
{
  bm::game_logic::PickupType type_;
};

struct BombData
{
  std::optional<unsigned int> parent_entity_id_;
  BombPrototype prototype_;
};

enum class NPCState
{
  random_walk,
  chasing_target,
};

struct NPCData
{
  NPCState goal;
  unsigned target_id;
  unsigned ticks_to_change{ 0 };
  std::vector<glm::vec2> trajectory;
};

} // namespace bm::game_logic
} // namespace bm