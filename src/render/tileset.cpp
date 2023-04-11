#include <utils/json.hpp>

#include <render/loader.hpp>
#include <render/tileset.hpp>
#include <utils/color.hpp>

using namespace render;

auto
Tileset::load_tileset(const std::filesystem::path& file)
  -> std::shared_ptr<render::Tileset>
{
  auto tileset = std::make_shared<render::Tileset>();

  auto tiles = utils::read_json(file);

  std::optional<utils::Color> transparent_color;
  if (tiles.contains("transparentcolor")) {
    transparent_color =
      utils::Color(tiles.at("transparentcolor").get<std::string>());
  }
  tileset->texture_ = render::load_texture_from_file(
    file.parent_path() / tiles.at("image"), transparent_color);

  tileset->tile_size_x_ =
    tiles.value("imagewidth", 1) / tiles.value("tilewidth", 1);
  tileset->tile_size_y_ =
    tiles.value("imageheight", 1) / tiles.value("tileheight", 1);
  tileset->total_tiles_ = tiles.value("tilecount", 1);

  if (tiles.contains("tiles")) {
    for (const auto tile : tiles.at("tiles")) {
      const auto id = tile.at("id").get<unsigned int>();
      const auto animation = tile.at("animation");

      Tileset::Animation result;
      for (const auto keypoint : animation) {
        result.sequence.emplace_back(Tileset::Animation::Keypoint{
          std::chrono::milliseconds{ keypoint.at("duration") },
          keypoint.at("tileid") });
      }
      tileset->animations.resize(id + 1);
      tileset->animations.at(id) = result;
    }
  }
  return tileset;
}