#pragma once

#include <bitset>
#include <optional>
#include <variant>

#include <bm/game_logic.hpp>
#include <glm/glm.hpp>
#include <render/tiled_map.hpp>
#include <utils/aabb.hpp>

namespace bm {
struct Entity
{
public:
  using EntityData = std::
    variant<std::monostate, game_logic::PlayerData, game_logic::PickupData>;

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
    frozen = 1,
    fireproof = 2,
    last
  };
  using Id = unsigned int;

public:
  Entity() = default;
  Entity(Id id, Type type)
    : id_{ id }
    , type_{ type }
  {
  }
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
  auto set_tileset(std::string tileset) -> Entity&
  {
    tile_.tileset_name_ = tileset;
    return *this;
  }

  auto set_tile(render::TiledMap::TileIndex tile_index) -> Entity&
  {
    tile_.tile_index_ = tile_index;
    tile_.animation_.reset();
    return *this;
  }

  auto set_animation(unsigned int animation_id) -> Entity&
  {
    tile_.animation_ = Entity::Tile::Animation{
      animation_id, 0, std::chrono::milliseconds{ 0 }
    };
    return *this;
  }

  auto set_data(EntityData data) -> Entity&
  {
    data_ = data;
    return *this;
  }

  auto get_id() const -> Id { return id_; }

public:
  // Generics
  Id id_;
  Type type_;
  std::bitset<Flags::last> flags_{ 0 };

  // Collision
  utils::AABB aabb_{ glm::vec2{ 0, 0 }, glm::vec2{ 1, 1 } };

  // TODO: kinematics
  float max_speed_{ 1.0 };
  glm::vec2 velocity_{ 0.0f, 0.0f };
  glm::vec2 acceleration_{ 0.0f, 0.0f };

  struct Controller
  {
    bool moving_left{ false };
    bool moving_right{ false };
    bool moving_down{ false };
    bool moving_up{ false };

    bool any() const
    {
      return moving_left or moving_right or moving_down or moving_up;
    }
  } controller_;

  // Visual component
  struct Tile
  {
    std::string tileset_name_{ "default" };
    render::TiledMap::TileIndex tile_index_{ 0 };
    struct Animation
    {
      unsigned int id;
      unsigned int keypoint_id;
      std::chrono::milliseconds remaining_time;
    };
    std::optional<Animation> animation_;
  } tile_;

  EntityData data_;
};

} // namespace bm