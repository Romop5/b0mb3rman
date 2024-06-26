#pragma once

#include <atomic>
#include <chrono>
#include <memory>

#include <render/interfaces/input_listener.hpp>
#include <render/interfaces/renderable.hpp>
#include <render/interfaces/renderable_observer.hpp>
#include <utils/exceptions.hpp>
#include <utils/manager.hpp>
#include <utils/raii_helpers.hpp>
#include <utils/type.hpp>

namespace render {

class Application
  : public render::interfaces::IInputListener
  , public render::interfaces::IRenderableObserver

{
public:
  class Settings
  {};

  explicit Application(render::interfaces::IRenderable& renderable);

  auto run() -> void;
  auto stop() -> void;

  virtual auto on_render(std::chrono::milliseconds delta) -> void;

  /* InputListener */
  virtual auto on_key_callback(int key, int scancode, int action, int mods)
    -> void override;

  /* IRenderableObserver */
  virtual auto on_viewport_change(float x, float y, float width, float height)
    -> void override;

private:
  render::interfaces::IRenderable& renderable_;
  std::atomic<bool> is_running_{ true };
};

} // namespace bm