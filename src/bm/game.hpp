#pragma once
#include <filesystem>
#include <string>

#include <bm/entity.hpp>
#include <bm/event_distributor.hpp>
#include <bm/game_controller.hpp>
#include <bm/world.hpp>

#include <nlohmann/json.hpp>
#include <render/application.hpp>
#include <render/font.hpp>
#include <render/tile_map_renderer.hpp>
#include <render/tile_renderer.hpp>
#include <render/viewport.hpp>

namespace bm {

class Game : public render::Application
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
  explicit Game(render::interfaces::IRenderable& renderable, Settings settings);

private:
  virtual auto on_render() -> void override;
  virtual auto on_key_callback(int key, int scancode, int action, int mods)
    -> void override;
  virtual auto on_viewport_change(float x, float y, float width, float height)
    -> void;

  auto start() -> void;

private:
  Settings settings_;
  render::Viewport viewport_;

  std::optional<render::TileRenderer> tile_renderer_;
  std::optional<render::TileMapRenderer> tile_map_renderer_;
  std::optional<render::Font> font_;

  EventDistributor event_distributor_;

  /// @brief Pool of dynamic entities
  World world_;

  /// @brief Manages game dynamics (update of entites & map)
  GameController game_controller_;
};

} // namespace bm