#pragma once

#include <memory>
#include <render/tileset.hpp>

namespace render {
struct TiledMap
{
  /// @brief Count of tiles in x direction
  unsigned int count_x{ 0 };

  /// @brief Count of tiles in y direction
  unsigned int count_y{ 0 };

  /// @brief Per-tile index to tileset
  std::vector<unsigned> tile_indices_;

  /// Definition of tiles
  std::shared_ptr<Tileset> tileset_;

  static auto load_map(const std::filesystem::path& file,
                       std::shared_ptr<render::Tileset> tileset)
    -> std::shared_ptr<render::TiledMap>;

public:
  auto validate() const;
};

} // namespace render