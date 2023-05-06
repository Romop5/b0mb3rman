#include <catch2/catch_test_macros.hpp>

#include <utils/color.hpp>

TEST_CASE("utils::Color: : constructor", "color")
{
  utils::Color color(1.0f, 1.0f, 1.0f, 0.0f);
  REQUIRE(color.a == 0);
  REQUIRE(color.r == 255);
  REQUIRE(color.g == 255);
  REQUIRE(color.b == 255);

  utils::Color color_str("#ffffff00");
  REQUIRE(color_str.a == 0);
  REQUIRE(color_str.r == 255);
  REQUIRE(color_str.g == 255);
  REQUIRE(color_str.b == 255);

  REQUIRE(color == color_str);

  utils::Color color_hex(0xffffff00);
  REQUIRE(color == color_hex);
  REQUIRE(color_str == color_hex);
}