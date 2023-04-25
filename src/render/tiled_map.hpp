#pragma once

#include <memory>
#include <variant>

#include <glm/glm.hpp>
#include <render/tileset.hpp>

namespace render {
struct TiledMap
{
public:
  using TileIndex = unsigned int;
  static constexpr TileIndex invalid_index = ~0;

  struct TileLayer
  {
    /// @brief Per-tile index to tileset (stored row-ordered)
    std::vector<TileIndex> tile_indices_;
  };

  struct ObjectLayer
  {};

  struct Layer
  {
    std::string name_;
    bool visible_{ true };
    std::variant<TileLayer, ObjectLayer> data_;
  };

public:
  static auto load_map(const std::filesystem::path& file,
                       std::shared_ptr<render::Tileset> tileset)
    -> std::shared_ptr<render::TiledMap>;

  auto validate() const -> void;

  /// Set tile
  auto tile(const glm::ivec2 position, TileIndex new_index, size_t layer_id = 0)
    -> void;

  /// Get tile
  auto tile(const glm::ivec2 position, size_t layer_id = 0) const -> TileIndex;

  auto has_layer(const std::string& name) const -> bool;
  auto get_layer(const std::string& name) const -> const Layer&;

  using TileLayerIterator =
    std::function<void(glm::ivec2 position, TileIndex index)>;
  auto iterate_tile_layer(const TileLayer& tile_layer,
                          TileLayerIterator iterator) -> void;

private:
  auto assert_position(const glm::ivec2 position) const -> void;
  auto assert_index(TileIndex index) const -> void;

public:
  /// @brief Count of tiles in x direction
  unsigned int count_x{ 0 };

  /// @brief Count of tiles in y direction
  unsigned int count_y{ 0 };

  std::vector<Layer> layers_;

  /// Definition of tiles
  std::shared_ptr<Tileset> tileset_;
};

} // namespace render