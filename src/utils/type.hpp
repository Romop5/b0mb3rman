#pragma once

namespace utils {

template<typename T>
struct NonMoveable
{
  NonMoveable() = default;
  NonMoveable(NonMoveable&&) = delete;
  NonMoveable& operator=(NonMoveable&&) = delete;

  NonMoveable(const NonMoveable&) = delete;
  NonMoveable& operator=(const NonMoveable&) = delete;
};

template<typename T>
struct NonCopyable
{
  NonCopyable() = default;
  NonCopyable(const NonCopyable&) = delete;
  NonCopyable& operator=(const NonCopyable&) = delete;

  NonCopyable(NonCopyable&&) = default;
  NonCopyable& operator=(NonCopyable&&) = default;
};
} // namespace utils