#pragma once

#include <tuple>

#include <glm/glm.hpp>

namespace utils {
namespace detail {

enum class IntervalComparisonResult
{
  a_inside_b,
  b_inside_a,
  overlaps,
  disjoint
};

template<typename T>
auto
compare_intervals(T a, T b) -> IntervalComparisonResult
{
  if (a.x <= b.x and a.y >= b.y) {
    return IntervalComparisonResult::b_inside_a;
  } else if (a.x >= b.x and a.y <= b.y) {
    return IntervalComparisonResult::a_inside_b;
  } else if ((a.x < b.x and a.y < b.y) or (a.x > b.x and a.y > b.y)) {
    return IntervalComparisonResult::disjoint;
  }
  return IntervalComparisonResult::overlaps;
}

} // namespace utils::detail

struct AABB
{
public:
  AABB() = default;
  AABB(glm::vec2 origin, glm::vec2 size)
    : origin_{ origin }
    , size_{ size }
  {
  }
  auto get_size() const -> glm::vec2 { return size_; }
  auto get_origin() const -> glm::vec2 { return origin_; }
  auto get_midpoint() const -> glm::vec2
  {
    return origin_ + size_ * glm::vec2(0.5);
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
      if (a_x[0] >= b_x[1])
        return false;
    }

    const auto a_y = project_y();
    const auto b_y = other.project_y();

    if (a_y[0] < b_y[0]) {
      if (a_y[1] <= b_y[0])
        return false;
    } else {
      if (a_y[0] >= b_y[1])
        return false;
    }
    return true;
  }

  auto contains(const glm::vec2& point) const -> bool
  {
    const auto this_x = project_x();
    const auto this_y = project_y();
    return ((point.x >= this_x[0] and point.x <= this_x[1]) and
            (point.y >= this_y[0] and point.y <= this_y[1]));
  }

  auto inside(const AABB& boundary) const -> bool
  {
    using namespace detail;
    const auto this_x = project_x();
    const auto this_y = project_y();

    const auto other_x = project_x();
    const auto other_y = project_y();

    if ((compare_intervals(this_x, other_x) ==
         IntervalComparisonResult::a_inside_b) and
        (compare_intervals(this_y, other_y) ==
         IntervalComparisonResult::a_inside_b)) {
      return true;
    }
    return false;
  }

  auto put_inside(const AABB& boundary) -> void
  {
    origin_ = glm::max(origin_, boundary.origin_);
    origin_ = glm::min(origin_, boundary.get_bottom_right() - size_);
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