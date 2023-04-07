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
  , tile_map_renderer_{ render::TileMapRenderer{ tile_renderer_ } }
  , font_renderer_{ viewport_, settings.assets_directory_ / "data-latin.ttf" }
  , hud_manager_{ font_renderer_ }
  , event_distributor_{}
  , world_{ event_distributor_ }
  , game_controller_{ event_distributor_, hud_manager_, world_ }
{
  load_level();
}

auto
Game::on_render(std::chrono::milliseconds delta) -> void
{
  /* Update world logic */
  event_distributor_.dispatch();
  world_.update(delta);
  world_.delete_marked_entities();

  /* Render the world */
  const auto screen_size = viewport_.get_size();
  tile_renderer_.set_projection_matrix(0, 0, screen_size[0], screen_size[1]);

  if (level_) {
    /* Draw static map */
    if (level_->map_) {
      tile_map_renderer_.render(*level_->map_);
    }

    const auto& default_tileset = level_->tilesets_.get_entity("default");
    if (default_tileset) {
      const auto tile_size = tile_map_renderer_.get_tile_size(*level_->map_);

      /* Draw dynamic entities */
      for (const auto& [id, entity] : world_) {
        const auto origin = entity.aabb_.origin_;
        const auto size = entity.aabb_.size_;

        const auto& tileset = level_->tilesets_.value_or(
          entity.tile_.tileset_name_, default_tileset);

        tile_renderer_.bind_tileset(*tileset);
        tile_renderer_.draw_quad(
          origin * tile_size, size * tile_size, entity.tile_.tile_index_);
      }
    } else {
      spdlog::error("Default tileset not defined!");
    }
  }

  /* Render overlays */
  hud_manager_.render(delta);
}

auto
Game::on_key_callback(int key, int scancode, int action, int mods) -> void
{
  // const auto delta = 1 / 10.0f;
  const auto delta = 1;
  auto movement = [&]() -> std::optional<glm::vec2> {
    glm::vec2 dir{ 0.0, 0.0 };
    if (action == GLFW_RELEASE or action == GLFW_REPEAT)
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

  auto movement_type =
    [&]() -> std::optional<bm::event::PlayerMoved::MoveDirection> {
    switch (key) {
      case GLFW_KEY_LEFT:
        return bm::event::PlayerMoved::MoveDirection::left;
      case GLFW_KEY_RIGHT:
        return bm::event::PlayerMoved::MoveDirection::right;
      case GLFW_KEY_DOWN:
        return bm::event::PlayerMoved::MoveDirection::down;
      case GLFW_KEY_UP:
        return bm::event::PlayerMoved::MoveDirection::up;
      default:
        return {};
    }
  }();

  if (movement_type and action != GLFW_REPEAT) {
    event_distributor_.enqueue_event(
      bm::event::PlayerMoved{ action == GLFW_PRESS, movement_type.value() });
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

auto
Game::load_level() -> void
{
  spdlog::debug("Game: initializing");
  level_ = std::make_unique<Level>(static_cast<nlohmann::json>(settings_));
  start();
}