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
    std::string tileset_name;
    std::string tilemap_path;
    std::vector<std::string> tilesets;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Settings,
                                                tileset_name,
                                                tilemap_path,
                                                tilesets)
  };

public:
  Level(std::filesystem::path assets_directory, Settings settings);

public:
  Settings settings_;
  std::shared_ptr<render::TiledMap> map_;
  utils::EntityNamedRegistry<std::shared_ptr<render::Tileset>> tilesets_;
};

} // namespace bm