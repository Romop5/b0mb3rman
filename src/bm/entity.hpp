#pragma once

#include <bitset>

#include <glm/glm.hpp>
#include <render/tiled_map.hpp>
#include <utils/aabb.hpp>

namespace bm {
struct Entity
{
public:
  enum Type : uint8_t
  {
    player,
    bomb,
    pickup,
    fire,
  };

  enum Flags
  {
    marked_for_destruction = 0,
    moved_last_tick = 1,
    last
  };
  using Id = unsigned int;

public:
  Entity() = default;
  Entity(Id id, Type type)
    : id_{ id }
    , type_{ type }
  {}
  auto set_max_speed(float value) -> Entity&
  {
    max_speed_ = value;
    return *this;
  }

  auto set_origin(glm::vec2 origin) -> Entity&
  {
    aabb_.origin_ = origin;
    return *this;
  }

  auto set_size(glm::vec2 size) -> Entity&
  {
    aabb_.size_ = size;
    return *this;
  }

  auto set_tile(render::TiledMap::TileIndex tile_index) -> Entity&
  {
    tile_.tile_index_ = tile_index;
    return *this;
  }

  auto get_id() const -> Id { return id_; }

public:
  Id id_;
  utils::AABB aabb_{ glm::vec2{ 0, 0 }, glm::vec2{ 1, 1 } };

  float max_speed_{ 1.0 };
  glm::vec2 velocity_{ 0.0f, 0.0f };
  glm::vec2 acceleration_{ 0.0f, 0.0f };

  struct Controller
  {
    bool moving_left{ false };
    bool moving_right{ false };
    bool moving_down{ false };
    bool moving_up{ false };
  } controller_;

  struct Tile
  {
    std::string tileset_name_{ "default" };
    render::TiledMap::TileIndex tile_index_{ 0 };
  } tile_;

  std::bitset<Flags::last> flags_{ 0 };
  Type type_;
};

} // namespace bm