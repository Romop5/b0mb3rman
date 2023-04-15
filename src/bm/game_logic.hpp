#pragma once

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
};

struct PlayerData
{
  unsigned int bomb_range_distance_{ 1 };
  unsigned int available_bomb_count_{ 1 };
};

struct PickupData
{
  bm::game_logic::PickupType type_;
};

struct BombData
{
  unsigned int parent_entity_id_;
  unsigned int range_;
};

} // namespace bm::game_logic
} // namespace bm