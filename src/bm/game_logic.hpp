#pragma once

namespace bm {

namespace game_logic {
enum PickupType
{
  increase_bomb_count,
  increase_bomb_range,
  freeze_for_some_time,
};

struct PlayerData
{
  int bomb_range_distance_{ 1 };
  int available_bomb_count_{ 1 };
};

struct PickupData
{
  bm::game_logic::PickupType type_;
};

struct BombData
{
  unsigned int parent_entity_id_{};
};

} // namespace bm::game_logic
} // namespace bm