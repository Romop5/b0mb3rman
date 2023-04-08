#include <bm/level.hpp>
#include <spdlog/spdlog.h>

using namespace bm;

Level::Level(std::filesystem::path assets_directory, Level::Settings settings)
  : settings_{ settings }

{
  const auto& assets = assets_directory;

  // Load initial tile texture and map
  spdlog::trace("Loading tileset '{}'", settings.tileset_name.c_str());
  auto tileset = render::Tileset::load_tileset(assets / settings.tileset_name);
  map_ = render::TiledMap::load_map(assets / settings.tilemap_path, tileset);
  tilesets_.create_named("default", tileset);

  // Load tilesets
  for (const auto& tileset_path : settings.tilesets) {
    spdlog::trace("Loading tileset '{}'", tileset_path);
    auto tileset = render::Tileset::load_tileset(assets / tileset_path);
    tilesets_.create_named(tileset_path, tileset);
  }
}