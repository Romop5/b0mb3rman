#include <catch2/catch_test_macros.hpp>

#include <utils/aabb.hpp>

constexpr auto distance_epsilon = 1e-6;

TEST_CASE("utils::AABB: intervals: disjoint", "aabb")
{
  using namespace utils::detail;

  REQUIRE(compare_intervals<glm::vec2>({ 0.0f, 1.0f }, { 2.0f, 3.0f }) ==
          IntervalComparisonResult::disjoint);
  REQUIRE(compare_intervals<glm::vec2>({ 0.0f, 1.0f }, { 1.1f, 3.0f }) ==
          IntervalComparisonResult::disjoint);
  REQUIRE(compare_intervals<glm::vec2>({ 1.0f, 2.0f }, { 0.0f, 0.9f }) ==
          IntervalComparisonResult::disjoint);
}

TEST_CASE("utils::AABB: intervals: overlaps", "aabb")
{
  using namespace utils::detail;
  REQUIRE(compare_intervals<glm::vec2>({ 0.0f, 1.0f }, { 0.5f, 1.5f }) ==
          IntervalComparisonResult::overlaps);

  REQUIRE(compare_intervals<glm::vec2>({ 1.0f, 2.5f }, { 0.0f, 2.0f }) ==
          IntervalComparisonResult::overlaps);
}

TEST_CASE("utils::AABB: intervals: a_inside_b", "aabb")
{
  using namespace utils::detail;
  REQUIRE(compare_intervals<glm::vec2>({ 1.0f, 2.0f }, { 0.0f, 3.0f }) ==
          IntervalComparisonResult::a_inside_b);

  REQUIRE(compare_intervals<glm::vec2>({ 0.0f, 3.0f }, { 1.0f, 2.0f }) ==
          IntervalComparisonResult::b_inside_a);

  REQUIRE(compare_intervals<glm::vec2>({ 0.1f, 2.9f }, { 0.0f, 3.0f }) ==
          IntervalComparisonResult::a_inside_b);

  REQUIRE(compare_intervals<glm::vec2>({ 0.0f, 3.0f }, { 0.1f, 2.9f }) ==
          IntervalComparisonResult::b_inside_a);

  REQUIRE(compare_intervals<glm::vec2>({ 0.01f, 2.99f }, { 0.0f, 3.0f }) ==
          IntervalComparisonResult::a_inside_b);

  REQUIRE(compare_intervals<glm::vec2>({ 0.0f, 3.0f }, { 0.01f, 2.99f }) ==
          IntervalComparisonResult::b_inside_a);
}

TEST_CASE("utils::AABB: constructors", "aabb")
{
  {
    utils::AABB aabb;
  }
  {
    const auto size = glm::vec2(40.0f, 20.0f);
    const auto origin = glm::vec2(100.0f, 200.0f);
    utils::AABB aabb(origin, size);

    REQUIRE(glm::distance(size, aabb.get_size()) < distance_epsilon);
    REQUIRE(glm::distance(origin, aabb.get_origin()) < distance_epsilon);

    REQUIRE(glm::distance(origin, aabb.get_top_left()) < distance_epsilon);

    REQUIRE(glm::distance(origin + size, aabb.get_bottom_right()) <
            distance_epsilon);
  }
}

TEST_CASE("utils::AABB: non-colliding", "aabb")
{
  // Two boxes, not intersecting
  // [0,0]
  //     ┌──┐
  //     │  │
  //     └──┘
  //     [2,2]┌──┐
  //          │  │
  //          └──┘

  utils::AABB upper({ 0.0f, 0.0f }, { 1.0f, 1.0f });
  utils::AABB down({ 2.0f, 2.0f }, { 1.0f, 1.0f });

  REQUIRE(not upper.collide(down));
  REQUIRE(not down.collide(upper));

  REQUIRE(not down.inside(upper));
  REQUIRE(not upper.inside(down));
}

TEST_CASE("utils::AABB: non-colliding,but touching", "aabb")
{
  // Two boxes, not intersecting
  // [0,0]
  //     ┌──┐
  //     │  │
  //     └──┘
  //     [1,1]┌──┐
  //          │  │
  //          └──┘

  utils::AABB upper({ 0.0f, 0.0f }, { 1.0f, 1.0f });
  utils::AABB down({ 1.0f, 1.0f }, { 1.0f, 1.0f });

  REQUIRE(not upper.collide(down));
  REQUIRE(not down.collide(upper));

  REQUIRE(not down.inside(upper));
  REQUIRE(not upper.inside(down));
}

TEST_CASE("utils::AABB: colliding", "aabb")
{
  // Two boxes, intersecting

  //[0,0]
  //    ┌──┐
  //    │┼┼┼┐
  //    └┼┼┼│
  //     └──┘

  utils::AABB upper({ 0.0f, 0.0f }, { 1.0f, 1.0f });
  utils::AABB down({ 0.5f, 0.5f }, { 1.0f, 1.0f });

  REQUIRE(upper.collide(down));
  REQUIRE(down.collide(upper));

  REQUIRE(not down.inside(upper));
  REQUIRE(not upper.inside(down));
}

TEST_CASE("utils::AABB: inside", "aabb")
{
  //
  //  ┌──────────────┐
  //  │              │
  //  │  ┌────────┐  │
  //  │  │        │  │
  //  │  │ inside │  │
  //  │  │        │  │
  //  │  └────────┘  │
  //  │              │
  //  └──────────────┘
  //        boundary

  utils::AABB boundary({ 0.0f, 0.0f }, { 3.0f, 3.0f });
  utils::AABB box({ 1.0f, 1.0f }, { 1.0f, 1.0f });

  REQUIRE(box.collide(boundary));
  REQUIRE(boundary.collide(box));

  REQUIRE(box.inside(boundary));
  REQUIRE(not boundary.inside(box));
}

TEST_CASE("utils::AABB: contains", "aabb")
{
  utils::AABB aabb({ 0.0f, 0.0f }, { 1.0f, 1.0f });

  REQUIRE(aabb.contains(glm::vec2(0, 0)));
  REQUIRE(aabb.contains(glm::vec2(1, 1)));
  REQUIRE(aabb.contains(glm::vec2(0, 1)));
  REQUIRE(aabb.contains(glm::vec2(1, 0)));

  REQUIRE(not aabb.contains(glm::vec2(1.1f, 1.1f)));
  REQUIRE(not aabb.contains(glm::vec2(-1.1f, -1.1f)));
  REQUIRE(not aabb.contains(glm::vec2(0, 1.1f)));
}
