#pragma once
#include <filesystem>
#include <string>

#include <nlohmann/json.hpp>
#include <render/application.hpp>
#include <render/font_renderer.hpp>
#include <render/tile_map_renderer.hpp>
#include <render/tile_renderer.hpp>
#include <render/viewport.hpp>

#include <bm/entity.hpp>
#include <bm/event_distributor.hpp>
#include <bm/game_controller.hpp>
#include <bm/hud_manager.hpp>
#include <bm/world.hpp>

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
  virtual auto on_render(std::chrono::milliseconds delta)
    -> void override final;
  virtual auto on_key_callback(int key, int scancode, int action, int mods)
    -> void override final;
  virtual auto on_viewport_change(float x, float y, float width, float height)
    -> void override final;

  auto start() -> void;

private:
  Settings settings_;

  render::Viewport viewport_;
  render::TileRenderer tile_renderer_;
  render::TileMapRenderer tile_map_renderer_;
  render::FontRenderer font_renderer_;

  HUDManager hud_manager_;
  EventDistributor event_distributor_;

  /// @brief Pool of dynamic entities
  World world_;

  /// @brief Manages game dynamics (update of entites & map)
  GameController game_controller_;
};

} // namespace bm