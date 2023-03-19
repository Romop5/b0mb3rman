#pragma once

#include <atomic>
#include <memory>

// #include <spdlog/spdlog.h>
// #include <vector>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <utils/exceptions.hpp>
#include <utils/manager.hpp>
#include <utils/raii_helpers.hpp>
#include <utils/type.hpp>

namespace render {

class Application : public std::enable_shared_from_this<Application>
{
public:
  class Settings
  {};

  Application() = default;

  auto initialize() -> void;

  auto run() -> void;

  auto stop() -> void;

  virtual auto on_render() -> void;
  virtual auto on_key_callback(int key, int scancode, int action, int mods)
    -> void;

private:
  utils::RaiiAction glfw_delete_system_;
  utils::RaiiOwnership<GLFWwindow> window_;
  utils::ManagerRaiiHandle<Application> application_handler_;

  std::atomic<bool> is_running_{ true };
};

} // namespace bm