#pragma once
#include <filesystem>
#include <string>

#include <application.hpp>
#include <nlohmann/json.hpp>
#include <render/tile_map_renderer.hpp>
#include <render/tile_renderer.hpp>

namespace bm {

class Game : public Application
{
public:
  struct Settings
  {
    std::filesystem::path assets_directory_;
    std::string tileset_name_;
    std::string tilemap_path_;
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Settings,
                                                    assets_directory_,
                                                    tileset_name_,
                                                    tilemap_path_)
  };

public:
  auto initialize(Settings settings) -> void;

private:
  virtual auto on_render() -> void override;
  virtual auto on_key_callback(int key, int scancode, int action, int mods)
    -> void override;

private:
  Settings settings_;

  std::optional<render::TileRenderer> tile_renderer_;
  std::optional<render::TileMapRenderer> tile_map_renderer_;

  class Player
  {
  public:
    auto update(const glm::vec2& movement)
    {
      origin_ += movement;
      if (origin_.x < 0)
        origin_.x = 0;
      if (origin_.y < 0)
        origin_.y = 0;
    }

    glm::vec2 origin_{ 0.0, 0.0 };
    glm::vec2 size_{ 50.0, 50.0 };

  } player_;
};

} // namespace bm