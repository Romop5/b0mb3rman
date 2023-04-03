#pragma once

#include <tuple>

#include <glm/glm.hpp>

namespace utils {
struct AABB
{
  AABB() = default;
  AABB(glm::vec2 origin, glm::vec2 size)
    : origin_{ origin }
    , size_{ size }
  {
  }

  auto get_top_left() const -> glm::vec2 { return origin_; }
  auto get_top_right() const -> glm::vec2
  {
    return glm::vec2(origin_.x + size_.x, origin_.y);
  }

  auto get_bottom_left() const -> glm::vec2
  {
    return glm::vec2(origin_.x, origin_.y + size_.y);
  }

  auto get_bottom_right() const -> glm::vec2 { return origin_ + size_; }

  auto project_x() const -> glm::vec2
  {
    return glm::vec2(origin_.x, origin_.x + size_.x);
  };

  auto project_y() const -> glm::vec2
  {
    return glm::vec2(origin_.y, origin_.y + size_.y);
  };

  /// @brief Determine this AABB with `other`
  /// @param other
  /// @return true if they collide
  auto collide(const AABB& other) const -> bool
  {
    const auto a_x = project_x();
    const auto b_x = other.project_x();

    if (a_x[0] < b_x[0]) {
      if (a_x[1] <= b_x[0])
        return false;
    } else {
      if (a_x[0] > b_x[1])
        return false;
    }

    const auto a_y = project_y();
    const auto b_y = other.project_y();

    if (a_y[0] < b_y[0]) {
      if (a_y[1] <= b_y[0])
        return false;
    } else {
      if (a_y[0] > b_y[1])
        return false;
    }
    return true;
  }

  auto distance_l2(const AABB& other) const
  {
    return glm::distance(origin_, other.origin_);
  }

  auto distance_l1(const AABB& other) const
  {
    return glm::dot(glm::abs(origin_ - other.origin_), glm::vec2(1));
  }

  glm::vec2 origin_{ 0, 0 };
  glm::vec2 size_{ 1, 1 };
};

} // namespace utils