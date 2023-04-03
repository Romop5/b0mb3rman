#include <render/application.hpp>

#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace render;

Application::Application(interfaces::IRenderable& renderable)
  : renderable_{ renderable }
{
}

auto
Application::run() -> void
{
  auto last_tick = std::chrono::high_resolution_clock::now();

  is_running_ = true;
  while (is_running_) {
    using namespace gl;

    auto now = std::chrono::high_resolution_clock::now();
    const auto elapsed = now - last_tick;
    last_tick = now;
    on_render(std::chrono::duration_cast<std::chrono::milliseconds>(elapsed));
    renderable_.swap_buffers();
  }
}

auto
Application::stop() -> void
{
  is_running_ = false;
}

auto
Application::on_render(std::chrono::milliseconds delta) -> void
{
}

auto
Application::on_key_callback(int key, int scancode, int action, int mods)
  -> void

{
  if (key == GLFW_KEY_ESCAPE) {
    is_running_ = false;
  }
}

auto
Application::on_viewport_change(float x, float y, float width, float height)
  -> void

{
}
