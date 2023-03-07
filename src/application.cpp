#include <application.hpp>

#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>

using namespace bm;

auto
Application::initialize() -> void
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
    [](
      GLFWwindow* window, int key, int scancode, int action, int mods) -> void {
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

auto
Application::run() -> void
{
  is_running_ = true;

  while (is_running_ && !glfwWindowShouldClose(window_.get())) {
    static int counter = 0;
    counter = (counter + 1) % 100;

    using namespace gl;

    // Keep running
    gl::glClearColor(counter / 100.0, 0.0f, 0.0f, 1.0f);
    gl::glClear(gl::GL_COLOR_BUFFER_BIT | gl::GL_DEPTH_BUFFER_BIT);

    on_render();

    glfwSwapBuffers(window_.get());
    glfwPollEvents();
  }
}

auto
Application::stop() -> void
{
  is_running_ = false;
}

auto
Application::on_render() -> void
{}

auto
Application::on_key_callback(int key, int scancode, int action, int mods)
  -> void

{
  if (key == GLFW_KEY_ESCAPE) {
    is_running_ = false;
  }
}
