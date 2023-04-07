#pragma once

#include <glm/glm.hpp>

namespace render {

class Viewport
{
public:
  Viewport() = default;

  auto get_origin() const -> glm::vec2 { return origin_; }
  auto get_size() const -> glm::vec2 { return size_; }

  auto origin(const glm::vec2 origin) -> void { origin_ = origin; }
  auto size(const glm::vec2 size) -> void { size_ = size; }

private:
  glm::vec2 origin_{ 0 };
  glm::vec2 size_{ 640, 480 };
};
} // namespace