#include <utils/json.hpp>

#include <render/tiled_map.hpp>

using namespace render;

namespace {
auto
transform_index(TiledMap::TileIndex& index) -> void
{
  if (index == 0) {
    index = TiledMap::invalid_index;
  } else {
    index--;
  }
}

auto
parse_layer(const nlohmann::json& layer_definition,
            render::TiledMap::TileLayer tag)
{
  decltype(tag) result;
  result.tile_indices_ =
    layer_definition.at("data").get<std::vector<unsigned>>();

  std::for_each(
    result.tile_indices_.begin(), result.tile_indices_.end(), transform_index);
  return result;
}

auto
parse_layer(const nlohmann::json& layer_definition,
            render::TiledMap::ObjectLayer tag)
{
  return tag;
}
} // namespace

auto
TiledMap::load_map(const std::filesystem::path& file,
                   std::shared_ptr<render::Tileset> tileset)
  -> std::shared_ptr<render::TiledMap>
{
  auto tilemap = std::make_shared<render::TiledMap>();
  auto map = utils::read_json(file);
  {
    tilemap->tileset_ = tileset;

    for (const auto& layer_definition : map.at("layers")) {
      Layer layer;
      layer.name_ = layer_definition.at("name");
      layer.visible_ = layer_definition.at("visible");

      bool is_tilelayer = layer_definition.at("type") == "tilelayer";
      if (is_tilelayer) {
        layer.data_ = parse_layer(layer_definition, TiledMap::TileLayer{});
      } else {
        layer.data_ = parse_layer(layer_definition, TiledMap::ObjectLayer{});
      }
      tilemap->layers_.push_back(layer);
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

  for (const auto& layer : layers_) {
    if (not std::holds_alternative<TileLayer>(layer.data_)) {
      continue;
    }
    const auto& data = std::get<TileLayer>(layer.data_);
    if (data.tile_indices_.size() != total_tiles) {
      throw std::runtime_error(
        fmt::format("Invalid TiledMap: totalTiles ({}) != count_x*count_y ({})",
                    data.tile_indices_.size(),
                    total_tiles));
    }
  }
}

auto
TiledMap::tile(const glm::ivec2 position, TileIndex new_index, size_t layer_id)
  -> void
{
  assert_position(position);
  assert_index(new_index);

  auto& layer = layers_.at(layer_id);
  if (not std::holds_alternative<TiledMap::TileLayer>(layer.data_)) {
    throw std::runtime_error(
      fmt::format("Layer {} is not a tile layer", layer_id));
  }
  auto& data = std::get<TiledMap::TileLayer>(layer.data_);
  data.tile_indices_[position.x + position.y * count_y] = new_index;
}

auto
TiledMap::tile(const glm::ivec2 position, size_t layer_id) const -> TileIndex
{
  assert_position(position);
  const auto& layer = layers_.at(layer_id);
  if (not std::holds_alternative<TiledMap::TileLayer>(layer.data_)) {
    throw std::runtime_error(
      fmt::format("Layer {} is not a tile layer", layer_id));
  }
  const auto& data = std::get<TiledMap::TileLayer>(layer.data_);
  return data.tile_indices_[position.x + position.y * count_y];
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