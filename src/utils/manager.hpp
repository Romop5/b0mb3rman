#pragma once

#include <memory>
#include <mutex>
#include <unordered_map>

namespace utils {
/**
 * @brief Thread-safe ownership-aware singleton binding between raw-ptr and C++
 * instances
 *
 * An instance can register in global `Manager`. Then, it's weak_ptr can be
 * obtained by searching the raw pointer.
 *
 * This mechanism is useful e.g. when using windows and events from
 * GLFW that can store a raw ptr to the instance as a part of window.
 *
 * @see ManagerRaiiHandle
 */
template<typename T>
class Manager
{
public:
  using Handle = void*;

private:
  Manager() = default;

public:
  static auto get() -> Manager&;

  /// Register a new instance
  [[nodiscard]] auto add(std::weak_ptr<T> instance) -> Handle;

  /// Removes instance (assumes a valid handle)
  auto remove(Handle handle) -> void;

  // Returns the instance ptr with ownership or nullptr
  [[nodiscard]] auto search(void* handle) -> std::shared_ptr<T>;

private:
  std::mutex instances_map_mutex_;
  std::unordered_map<void*, std::weak_ptr<T>> instances_map_;
};

///////////////////////////////////////////////////////////////////////////////
template<typename T>
auto
Manager<T>::get() -> Manager<T>&
{
  static std::unique_ptr<Manager> manager;
  if (!manager) {
    manager = std::unique_ptr<Manager>{ new Manager() };
  }
  return *manager.get();
}

template<typename T>
auto
Manager<T>::add(std::weak_ptr<T> instance) -> Handle
{
  auto lock = std::unique_lock{ instances_map_mutex_ };

  auto sp = instance.lock();
  if (not sp.get()) {
    throw std::runtime_error("registering empty instance weak_ptr");
  }

  const auto handle = sp.get();
  instances_map_[handle] = std::move(instance);
  return handle;
}

template<typename T>
auto
Manager<T>::remove(Handle handle) -> void
{
  auto lock = std::unique_lock{ instances_map_mutex_ };

  if (not handle) {
    throw std::runtime_error(
      "internal error: nullptr handle in Manager::remove()");
  }
  instances_map_.erase(handle);
}

template<typename T>
auto
Manager<T>::search(void* handle) -> std::shared_ptr<T>
{
  if (instances_map_.find(handle) == instances_map_.end()) {
    return {};
  }
  return instances_map_.at(handle).lock();
}

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Owns `Manager` registration handle
 *
 * A RAII helper for Manager.
 * @see Manager
 */
template<typename T>
class ManagerRaiiHandle
{
public:
  ManagerRaiiHandle() = default;
  ManagerRaiiHandle(std::weak_ptr<T>&& weak_ptr)
  {
    handle_ = Manager<T>::get().add(weak_ptr);
  }

  ManagerRaiiHandle(const ManagerRaiiHandle& other) = delete;
  ManagerRaiiHandle& operator=(const ManagerRaiiHandle& other) = delete;

  ManagerRaiiHandle(ManagerRaiiHandle&& other)
  {
    std::swap(other.handle_, handle_);
  }
  ManagerRaiiHandle& operator=(ManagerRaiiHandle&& other)
  {
    std::swap(other.handle_, handle_);
    return *this;
  }

  ~ManagerRaiiHandle() noexcept
  {
    if (handle_) {
      Manager<T>::get().remove(handle_);
    }
    handle_ = nullptr;
  }

private:
  typename Manager<T>::Handle handle_ = nullptr;
};

} // namespace utils