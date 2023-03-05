#include <atomic>
#include <memory>
#include <spdlog/spdlog.h>

#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <utils/exceptions.hpp>
#include <utils/raii_helpers.hpp>

namespace bm {

class Application
{
public:
  class Settings
  {};

  Application()
  {
    // Initialize GLFW
    utils::throw_runtime_on_false(glfwInit(), "Failed to create GLFW");

    glfw_delete_system_ = utils::make_raii_action([]() { glfwTerminate(); });

    // Create a GLFW window
    window_ = utils::make_raii_deleter<GLFWwindow>(
      glfwCreateWindow(640, 480, "My Title", NULL, NULL),
      [](GLFWwindow* ptr) -> void { glfwDestroyWindow(ptr); });
    utils::throw_runtime_on_false(window_, "Failed to create a GLFW window");

    glfwMakeContextCurrent(window_.get());

    glfwSetKeyCallback(
      window_.get(),
      [](GLFWwindow* window, int key, int scancode, int action, int mods)
        -> void { glfwDestroyWindow(window); });
    glfwSetFramebufferSizeCallback(
      window_.get(), [](GLFWwindow* window, int width, int height) -> void {
        gl::glViewport(0, 0, width, height);
      });
    // Initialize OpenGL
    glbinding::initialize(glfwGetProcAddress);
  }

  auto run(std::function<void()> render_callback = []() {}) -> void
  {

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

private:
  std::atomic<bool> is_running_;
  utils::RaiiAction glfw_delete_system_;
  utils::RaiiOwnership<GLFWwindow> window_;
};

} // namespace bm