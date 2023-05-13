#include <catch2/catch_test_macros.hpp>

#include <utils/occupancy_map.hpp>

TEST_CASE("utils::Occupancy: : trivial", "occupancy_map")
{
  utils::OccupancyMap2D<bool> occupancy_map({ 0, 0 }, false);
  const auto size = occupancy_map.size();
  REQUIRE(size == 0);
}

TEST_CASE("utils::Occupancy: : basic", "occupancy_map")
{
  using Map = utils::OccupancyMap2D<bool>;
  Map occupancy_map({ 2, 3 }, false);
  const auto size = occupancy_map.size();
  REQUIRE(size == 6);
  REQUIRE(occupancy_map.at({ 0, 0 }) == false);
  REQUIRE(occupancy_map.at({ 1, 0 }) == false);
  REQUIRE(occupancy_map.at({ 0, 1 }) == false);
  REQUIRE(occupancy_map.at({ 1, 1 }) == false);
  REQUIRE(occupancy_map.at({ 0, 2 }) == false);
  REQUIRE(occupancy_map.at({ 1, 2 }) == false);

  occupancy_map.at({ 1, 2 }) = true;

  REQUIRE(occupancy_map.at({ 1, 2 }) == true);

  occupancy_map.for_each([](const auto index, const auto value) {
    if (index == Map::Index{ 1, 2 }) {
      REQUIRE(value == true);
    } else {

      REQUIRE(value == false);
    }
  });
}
