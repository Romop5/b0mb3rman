#pragma once

#include <chrono>
#include <filesystem>
#include <render/resource.hpp>

namespace render {
/**
 * @brief Defines mapping for tile system to visual texture
 *
 */
struct Tileset
{
  struct Animation
  {
    struct Keypoint
    {
      std::chrono::milliseconds duration;
      unsigned int tile;
    };
    std::vector<Keypoint> sequence;
  };

  std::vector<Animation> animations;
  unsigned int tile_size_x_{ 0 };
  unsigned int tile_size_y_{ 0 };
  // unsigned int tiles_per_row_{0};
  unsigned int total_tiles_{ 0 };
  Texture texture_;

  static auto load_tileset(const std::filesystem::path& file)
    -> std::shared_ptr<render::Tileset>;
};
} // namespace render
