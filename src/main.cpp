#include <iostream>
#include <string>

#include <lyra/lyra.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <application.hpp>
#include <render/resource.hpp>
#include <render/tile_program.hpp>
#include <render/tile_renderer.hpp>
#include <utils/io.hpp>
#include <utils/json.hpp>

using namespace gl;

int
main(int argc, const char* argv[])
{
  spdlog::set_level(spdlog::level::debug);

  // Parse arguments
  std::string tileset_path = "cavesofgallet_tiles.json";
  std::string tilemap_path = "map.json";
  auto cli = lyra::cli() |
             lyra::opt(tileset_path, "tileset_path")["-t"]["--tileset_path"](
               "Path to tile set definition (JSON)") |
             lyra::opt(tilemap_path, "tilemap_path")["-t"]["--tilemap_path"](
               "Path to tile map definition (JSON)");

  const auto result = cli.parse({ argc, argv });
  if (!result) {
    spdlog::error("Failed to parse cli");
    return -1;
  }

  try {
    auto app = std::make_shared<bm::Application>();
    app->initialize();

    auto assets = std::filesystem::path{ "./assets" };

    auto tileset = render::Tileset::load_tileset(assets / tileset_path);

    auto tilemap =
      render::TiledMap::load_map(assets / tilemap_path, std::move(tileset));

    auto program = render::Program{};
    [&]() {
      auto vs_shader_code = utils::read_text_file(assets / "tm.vs.glsl");
      auto vs_shader =
        render::load_shader(gl::GL_VERTEX_SHADER, vs_shader_code);

      auto fs_shader_code = utils::read_text_file(assets / "tm.fg.glsl");
      auto fs_shader =
        render::load_shader(gl::GL_FRAGMENT_SHADER, fs_shader_code);

      auto shaders = std::vector<render::Shader>{};
      shaders.emplace_back(std::move(vs_shader));
      shaders.emplace_back(std::move(fs_shader));
      program = render::link_program(std::move(shaders));
    }();

    render::TileRenderer renderer{ std::move(program) };
    renderer.add_map("test", tilemap);

    spdlog::info("Starting inifinite loop");
    app->run([&]() {
      renderer.render();
      // app.stop();
    });

    spdlog::info("Terminating ...");
  } catch (std::exception& e) {
    spdlog::critical("Application exception: {}", e.what());
    return 1;
  }

  return 0;
}