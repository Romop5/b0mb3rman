#include <bm/game.hpp>

#include <render/resource.hpp>
#include <render/tile_program.hpp>
#include <utils/io.hpp>
#include <utils/json.hpp>

using namespace bm;
using namespace render;

Game::Game()
  : event_distributor_{}
  , game_controller_{ event_distributor_ }
{
}

auto
Game::initialize(Settings settings) -> void
{

  Application::initialize();

  const auto& assets = settings.assets_directory_;

  // Construct tile renderer
  auto program = render::Program{};
  [&]() {
    auto vs_shader_code = utils::read_text_file(assets / "tm.vs.glsl");
    auto vs_shader = render::load_shader(gl::GL_VERTEX_SHADER, vs_shader_code);

    auto fs_shader_code = utils::read_text_file(assets / "tm.fg.glsl");
    auto fs_shader =
      render::load_shader(gl::GL_FRAGMENT_SHADER, fs_shader_code);

    auto shaders = std::vector<render::Shader>{};
    shaders.emplace_back(std::move(vs_shader));
    shaders.emplace_back(std::move(fs_shader));
    program = render::link_program(shaders);
  }();

  tile_renderer_ = render::TileRenderer{ std::move(program) };
  tile_map_renderer_.emplace(render::TileMapRenderer{ *tile_renderer_ });

  // Load initial tile texture and map
  auto tileset = render::Tileset::load_tileset(assets / settings.tileset_name_);

  auto tilemap = render::TiledMap::load_map(assets / settings.tilemap_path_,
                                            std::move(tileset));
  tile_map_renderer_->add_map("default", tilemap);
}

auto
Game::on_render() -> void
{
  const auto screen_size = tile_renderer_->get_screen_size();
  tile_renderer_->set_projection_matrix(0, 0, screen_size[0], screen_size[1]);

  tile_map_renderer_->render();

  const auto tile_size = tile_map_renderer_->get_tile_size();
  tile_renderer_->draw_quad(player_.origin_ * tile_size, tile_size, 10);
}

auto
Game::on_key_callback(int key, int scancode, int action, int mods) -> void
{
  // const auto delta = 1 / 10.0f;
  const auto delta = 1;
  auto movement = [&]() -> std::optional<glm::vec2> {
    glm::vec2 dir{ 0.0, 0.0 };
    switch (key) {
      case GLFW_KEY_LEFT: {
        dir = { -1.0, 0.0 };
      } break;

      case GLFW_KEY_RIGHT: {
        dir = { 1.0, 0.0 };
      } break;

      case GLFW_KEY_DOWN: {
        dir = { 0.0, 1.0 };
      } break;

      case GLFW_KEY_UP: {
        dir = { 0.0, -1.0 };
      } break;
      default:
        return {};
    }
    dir *= delta;
    return dir;
  }();

  if (movement) {
    player_.update(*movement);
  }

  Application::on_key_callback(key, scancode, action, mods);
}