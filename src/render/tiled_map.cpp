#include <utils/json.hpp>

#include <render/tiled_map.hpp>

using namespace render;

auto
TiledMap::load_map(const std::filesystem::path& file,
                   std::shared_ptr<render::Tileset> tileset)
  -> std::shared_ptr<render::TiledMap>
{
  auto tilemap = std::make_shared<render::TiledMap>();
  auto map = utils::read_json(file);
  {
    tilemap->tileset_ = tileset;
    tilemap->tile_indices_ =
      map.at("layers").at(0).at("data").get<std::vector<unsigned>>();
    for (auto& tile : tilemap->tile_indices_) {
      tile = tile - 1;
    }

    tilemap->count_x = map.at("width");
    tilemap->count_y = map.at("height");
  }
  return tilemap;
}

auto
TiledMap::validate() const -> void
{
  const auto total_tiles = count_x * count_y;
  if (tile_indices_.size() != total_tiles) {
    throw std::runtime_error(
      fmt::format("Invalid TiledMap: totalTiles ({}) != count_x*count_y ({})",
                  tile_indices_.size(),
                  total_tiles));
  }
}

auto
TiledMap::tile(const glm::ivec2 position, TileIndex new_index) -> void
{
  assert_position(position);
  assert_index(new_index);
  tile_indices_[position.x + position.y * count_y] = new_index;
}

auto
TiledMap::tile(const glm::ivec2 position) const -> TileIndex
{
  assert_position(position);
  return tile_indices_[position.x + position.y * count_y];
}

auto
TiledMap::assert_position(const glm::ivec2 position) const -> void
{
  if (position.x >= count_x or position.y >= count_y) {
    throw std::runtime_error(fmt::format(
      "validate_position: invalid position ({},{})", position.x, position.y));
  }
}

auto
TiledMap::assert_index(TileIndex index) const -> void
{
  if (index >= tileset_->total_tiles_) {
    {
      throw std::runtime_error(
        fmt::format("validate_index: invalid tile index {}: total tiles: {}",
                    index,
                    tileset_->total_tiles_));
    }
  }
}