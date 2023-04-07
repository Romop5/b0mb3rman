#pragma once

#include <vector>
#include <optional>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <render/interfaces/input_listener.hpp>
#include <render/interfaces/renderable.hpp>
#include <render/interfaces/renderable_observer.hpp>
#include <utils/manager.hpp>
#include <utils/raii_helpers.hpp>

namespace render {

class Window
  : public interfaces::IRenderable
  , public interfaces::IRenderableObserver
  , public interfaces::IInputListener
  , public std::enable_shared_from_this<Window>
{
public:
  Window();

  auto register_callbacks() -> void;
  auto set_renderable_observer(interfaces::IRenderableObserver& observer)
    -> void;
  auto set_input_observer(interfaces::IInputListener& observer) -> void;

  /* IRenderable*/
  auto swap_buffers() -> void override;

  /* IRenderableObserver*/
  auto on_viewport_change(float x, float y, float width, float height)
    -> void override;

  /* IInputListener*/
  auto on_key_callback(int key, int scancode, int action, int mods)
    -> void override;

private:
  utils::RaiiAction glfw_delete_system_;
  utils::RaiiOwnership<GLFWwindow> window_;
  utils::ManagerRaiiHandle<Window> window_handler_;
  std::optional<std::reference_wrapper<interfaces::IRenderableObserver>>
    observer_renderable_;
  std::optional<std::reference_wrapper<interfaces::IInputListener>>
    observer_inputlistener_;
};

} // namespace bm