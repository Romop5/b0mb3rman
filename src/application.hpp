#include <atomic>
#include <memory>
#include <spdlog/spdlog.h>
#include <vector>

#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <utils/exceptions.hpp>
#include <utils/manager.hpp>
#include <utils/raii_helpers.hpp>
#include <utils/type.hpp>

namespace bm {

class Application : public std::enable_shared_from_this<Application>
{
public:
  class Settings
  {};

  Application() = default;

  auto initialize()
  {
    // Register application globally
    application_handler_ = { weak_from_this() };

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

    glfwSetKeyCallback(
      window_.get(),
      [](GLFWwindow* window, int key, int scancode, int action, int mods)
        -> void {
        auto app_ = utils::Manager<Application>::get().search(
          glfwGetWindowUserPointer(window));
        if (app_) {
          app_->on_key_callback(key, scancode, action, mods);
        }
      });

    glfwSetFramebufferSizeCallback(
      window_.get(), [](GLFWwindow* window, int width, int height) -> void {
        gl::glViewport(0, 0, width, height);
      });

    // Initialize OpenGL
    glbinding::initialize(glfwGetProcAddress);
  }

  auto run(std::function<void()> render_callback = []() {}) -> void
  {
    is_running_ = true;

    while (is_running_ && !glfwWindowShouldClose(window_.get())) {
      static int counter = 0;
      counter = (counter + 1) % 100;

      using namespace gl;

      // Keep running
      gl::glClearColor(counter / 100.0, 0.0f, 0.0f, 1.0f);
      gl::glClear(gl::GL_COLOR_BUFFER_BIT | gl::GL_DEPTH_BUFFER_BIT);

      render_callback();

      glfwSwapBuffers(window_.get());
      glfwPollEvents();
    }
  }

  auto stop() { is_running_ = false; }

  auto on_key_callback(int key, int scancode, int action, int mods) -> void
  {
    is_running_ = false;
  }

private:
  utils::RaiiAction glfw_delete_system_;
  utils::RaiiOwnership<GLFWwindow> window_;
  utils::ManagerRaiiHandle<Application> application_handler_;

  std::atomic<bool> is_running_{ true };
};

} // namespace bm