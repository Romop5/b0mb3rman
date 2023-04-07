#include <bm/level.hpp>

using namespace bm;

Level::Level(Settings settings)
  : settings_{ settings }

{
  const auto& assets = settings.assets_directory_;

  // Load initial tile texture and map
  auto tileset = render::Tileset::load_tileset(assets / settings.tileset_name_);
  map_ = render::TiledMap::load_map(assets / settings.tilemap_path_, tileset);

  tilesets_.create_named("default", tileset);
}