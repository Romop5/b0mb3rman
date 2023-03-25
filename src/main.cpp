#include <iostream>
#include <string>

#include <lyra/lyra.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/cfg/env.h>
#include <spdlog/spdlog.h>

#include <bm/game.hpp>
#include <render/window.hpp>

enum ReturnCodes
{
  success = 0,
  parsing_error = 1,
  runtime_error = 2
};

int
main(int argc, const char* argv[])
{
  spdlog::set_level(spdlog::level::trace);
  spdlog::cfg::load_env_levels();

  bm::Game::Settings settings;
  settings.assets_directory_ = std::filesystem::path{ "./assets" };
  settings.tileset_name_ = "cavesofgallet_tiles.json";
  settings.tilemap_path_ = "map.json";

  // Parse arguments
  auto cli =
    lyra::cli() |
    lyra::opt(settings.tileset_name_, "tileset_path")["-t"]["--tileset_path"](
      "Path to tile set definition (JSON)") |
    lyra::opt(settings.tilemap_path_, "tilemap_path")["-t"]["--tilemap_path"](
      "Path to tile map definition (JSON)");

  const auto result = cli.parse({ argc, argv });
  if (!result) {
    spdlog::error("Failed to parse cli");
    return ReturnCodes::parsing_error;
  }

  // Initialize and run the game
  try {
    auto window = std::make_shared<render::Window>();
    auto app = bm::Game{ *window, settings };
    window->register_callbacks();
    window->set_input_observer(app);
    window->set_renderable_observer(app);

    spdlog::info("Starting inifinite loop");
    app.run();
    spdlog::info("Terminating ...");

  } catch (std::exception& e) {
    spdlog::critical("Application exception: {}", e.what());
    return ReturnCodes::runtime_error;
  }

  return ReturnCodes::success;
}
