#include <utils/json.hpp>

#include <render/loader.hpp>
#include <render/tileset.hpp>

using namespace render;

auto
Tileset::load_tileset(const std::filesystem::path& file)
  -> std::shared_ptr<render::Tileset>
{
  auto tileset = std::make_shared<render::Tileset>();

  auto tiles = utils::read_json(file);

  tileset->texture_ =
    render::load_texture_from_file(file.parent_path() / tiles.at("image"));

  tileset->tile_size_x_ =
    tiles.value("imagewidth", 1) / tiles.value("tilewidth", 1);
  tileset->tile_size_y_ =
    tiles.value("imageheight", 1) / tiles.value("tileheight", 1);
  tileset->total_tiles_ = tiles.value("tilecount", 1);

  return tileset;
}