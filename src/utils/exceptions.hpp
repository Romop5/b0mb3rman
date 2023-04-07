
#pragma once

#include <stdexcept>
#include <exception>
#include <string>

namespace utils {
template<typename T>
auto
throw_runtime_on_false(T state, const std::string&& message) -> void
{
  if (!state) {
    throw std::runtime_error(message);
  }
}

class not_implemented : public std::runtime_error
{
public:
  not_implemented() = default;
  not_implemented(const std::string& str)
    : std::runtime_error{ str }
  {}
};
} // namespace utils