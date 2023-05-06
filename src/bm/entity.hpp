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
  using EntityData = std::variant<std::monostate,
                                  game_logic::PlayerData,
                                  game_logic::PickupData,
                                  game_logic::BombData,
                                  game_logic::NPCData>;

  enum Type : uint8_t
  {
    player,
    npc,
    bomb,
    pickup,
    fire,
    crate,
    particle,
    last_type
  };
  using TypeMask = std::bitset<Type::last_type>;

  enum Flags
  {
    marked_for_destruction = 0,
    unbounded = 1,
    frozen = 2,
    fireproof = 3,
    last_flag
  };
  using FlagsBitset = std::bitset<Flags::last_flag>;

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

  auto set_collision_mask(TypeMask mask) -> Entity&
  {
    collision_mask_ = mask;
    return *this;
  }

  auto set_collision_mask_bit(Type bit) -> Entity&
  {
    collision_mask_.set(static_cast<unsigned>(bit));
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

  auto set_flags(Entity::Flags flag) -> Entity&
  {
    flags_.set(static_cast<unsigned>(flag));
    return *this;
  }

  auto get_id() const -> Id { return id_; }
  auto get_type() const -> Type { return type_; }

public:
  // Generics
  Id id_;
  Type type_;
  FlagsBitset flags_{ 0 };

  // Collision
  utils::AABB aabb_{ glm::vec2{ 0, 0 }, glm::vec2{ 1, 1 } };
  TypeMask collision_mask_{ 0 };

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