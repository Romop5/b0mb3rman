#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>
#include <render/window.hpp>

#include <utils/exceptions.hpp>

render::Window::Window()
{
  // Initialize GLFW
  utils::throw_runtime_on_false(glfwInit(), "Failed to create GLFW");

  glfw_delete_system_ = utils::make_raii_action([]() { glfwTerminate(); });

  // glfwWindowHint(GLFW_SAMPLES, 0);

  // Create a GLFW window
  window_ = utils::make_raii_deleter<GLFWwindow>(
    glfwCreateWindow(640, 480, "My Title", NULL, NULL),
    [](GLFWwindow* ptr) -> void { glfwDestroyWindow(ptr); });
  utils::throw_runtime_on_false(window_, "Failed to create a GLFW window");

  // Requires application to be NonMoveable
  glfwSetWindowUserPointer(window_.get(), static_cast<void*>(this));
  glfwMakeContextCurrent(window_.get());

  // Initialize OpenGL
  glbinding::initialize(glfwGetProcAddress);
}

auto
render::Window::register_callbacks() -> void
{
  // Register window globally
  window_handler_ = { weak_from_this() };

  glfwSetKeyCallback(
    window_.get(),
    [](
      GLFWwindow* window, int key, int scancode, int action, int mods) -> void {
      auto managed_window =
        utils::Manager<Window>::get().search(glfwGetWindowUserPointer(window));
      if (managed_window) {
        managed_window->on_key_callback(key, scancode, action, mods);
      }
    });

  glfwSetFramebufferSizeCallback(
    window_.get(), [](GLFWwindow* window, int width, int height) -> void {
      gl::glViewport(0, 0, width, height);
      auto managed_window =
        utils::Manager<Window>::get().search(glfwGetWindowUserPointer(window));
      if (managed_window) {
        managed_window->on_viewport_change(0, 0, width, height);
      }
    });
}

auto
render::Window::set_renderable_observer(
  render::interfaces::IRenderableObserver& observer) -> void
{
  observer_renderable_ = observer;
}

auto
render::Window::set_input_observer(render::interfaces::IInputListener& observer)
  -> void
{
  observer_inputlistener_ = observer;
}

auto
render::Window::swap_buffers() -> void
{
  glfwSwapBuffers(window_.get());
  glfwPollEvents();
}

auto
render::Window::on_viewport_change(float x, float y, float width, float height)
  -> void
{
  if (observer_renderable_) {
    observer_renderable_->get().on_viewport_change(x, y, width, height);
  }
}

auto
render::Window::on_key_callback(int key, int scancode, int action, int mods)
  -> void
{
  if (observer_inputlistener_) {
    observer_inputlistener_->get().on_key_callback(key, scancode, action, mods);
  }
}