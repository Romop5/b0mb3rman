#pragma once

#include <glm/glm.hpp>
#include <render/tiled_map.hpp>

namespace bm {
struct Entity
{
public:
  using Id = unsigned int;
  virtual ~Entity() = default;

public:
  Id id_;
  glm::vec2 origin_{ 0.0, 0.0 };
  glm::vec2 size_{ 1.0, 1.0 };
  render::TiledMap::TileIndex tile_index_{ 0 };
};

} // namespace bm