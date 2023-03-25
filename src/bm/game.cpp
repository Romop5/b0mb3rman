#include <bm/game.hpp>

#include <render/resource.hpp>
#include <render/tile_program.hpp>
#include <utils/io.hpp>
#include <utils/json.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace bm;
using namespace render;

namespace {
auto
load_tile_renderer_program(std::filesystem::path assets) -> render::Program
{
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
  return std::move(program);
}
} // namespace

Game::Game(interfaces::IRenderable& renderable, Settings settings)
  : Application{ renderable }
  , settings_{ settings }
  , viewport_{}
  , tile_renderer_{ render::TileRenderer{
      viewport_,
      load_tile_renderer_program(settings.assets_directory_) } }
  , event_distributor_{}
  , world_{ event_distributor_ }
  , game_controller_{ event_distributor_, world_ }
{
  spdlog::debug("Game: initializing");

  tile_map_renderer_.emplace(render::TileMapRenderer{ *tile_renderer_ });
  const auto& assets = settings.assets_directory_;

  // tile_renderer_.emplace(std::move(program));

  // Create default font
  font_.emplace("arial.ttf");

  // Load initial tile texture and map
  auto tileset = render::Tileset::load_tileset(assets / settings.tileset_name_);

  auto tilemap = render::TiledMap::load_map(assets / settings.tilemap_path_,
                                            std::move(tileset));
  tile_map_renderer_->add_map("default", tilemap);

  start();
}

auto
Game::on_render() -> void
{
  event_distributor_.dispatch();
  world_.detect_collisions();

  // Render the world
  const auto screen_size = viewport_.get_size();
  tile_renderer_->set_projection_matrix(0, 0, screen_size[0], screen_size[1]);

  tile_map_renderer_->render();

  const auto tile_size = tile_map_renderer_->get_tile_size();

  for (auto& [id, entity] : world_) {
    const auto origin = entity.aabb_.origin_;
    const auto size = entity.aabb_.size_;
    tile_renderer_->draw_quad(
      origin * tile_size, size * tile_size, entity.tile_index_);
  }

  font_->draw_text("ok lets go", glm::vec2(0, 0));
  // font_->draw_text("ok", glm::vec2(100, 500));
  // font_->draw_text("okokok", glm::vec2(100, 1000));
  world_.delete_marked_entities();
}

auto
Game::on_key_callback(int key, int scancode, int action, int mods) -> void
{
  // const auto delta = 1 / 10.0f;
  const auto delta = 1;
  auto movement = [&]() -> std::optional<glm::vec2> {
    glm::vec2 dir{ 0.0, 0.0 };
    if (action == GLFW_RELEASE)
      return dir;
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
    event_distributor_.enqueue_event(bm::event::PlayerMoved{ *movement });
  }

  if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
    event_distributor_.enqueue_event(bm::event::BombPlanted{});
  }

  if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS) {
    start();
  }

  Application::on_key_callback(key, scancode, action, mods);
}

auto
Game::on_viewport_change(float x, float y, float width, float height) -> void
{
  viewport_.origin(glm::vec2(x, y));
  viewport_.size(glm::vec2(width, height));
}

auto
Game::start() -> void
{
  spdlog::debug("Game::start()");
  world_.clear();
  event_distributor_.clear();

  event_distributor_.enqueue_event(event::GameStarted{});
}