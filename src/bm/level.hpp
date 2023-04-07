#pragma once

#include <filesystem>
#include <memory>

#include <nlohmann/json.hpp>
#include <render/tiled_map.hpp>
#include <utils/entity_registry.hpp>

namespace bm {

class Level
{
public:
  struct Settings
  {
    std::filesystem::path assets_directory_;
    std::string tileset_name_;
    std::string tilemap_path_;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Settings,
                                                assets_directory_,
                                                tileset_name_,
                                                tilemap_path_)
  };

public:
  explicit Level(Settings settings);

public:
  Settings settings_;
  std::shared_ptr<render::TiledMap> map_;
  utils::EntityNamedRegistry<std::shared_ptr<render::Tileset>> tilesets_;
};

} // namespace bm