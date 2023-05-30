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

TEST_CASE("utils::Occupancy: : index", "occupancy_map")
{
  using Index2D = std::array<unsigned, 2>;

  REQUIRE(utils::compute_nd_index<Index2D, Index2D>(0, { 1, 1 }) ==
          Index2D{ 0, 0 });

  REQUIRE(utils::compute_nd_index<Index2D, Index2D>(0, { 2, 2 }) ==
          Index2D{ 0, 0 });
  REQUIRE(utils::compute_nd_index<Index2D, Index2D>(1, { 2, 2 }) ==
          Index2D{ 1, 0 });
  REQUIRE(utils::compute_nd_index<Index2D, Index2D>(2, { 2, 2 }) ==
          Index2D{ 0, 1 });
  REQUIRE(utils::compute_nd_index<Index2D, Index2D>(3, { 2, 2 }) ==
          Index2D{ 1, 1 });

  REQUIRE(utils::compute_nd_index<Index2D, Index2D>(0, { 3, 3 }) ==
          Index2D{ 0, 0 });
  REQUIRE(utils::compute_nd_index<Index2D, Index2D>(1, { 3, 3 }) ==
          Index2D{ 1, 0 });
  REQUIRE(utils::compute_nd_index<Index2D, Index2D>(2, { 3, 3 }) ==
          Index2D{ 2, 0 });
  REQUIRE(utils::compute_nd_index<Index2D, Index2D>(3, { 3, 3 }) ==
          Index2D{ 0, 1 });
}

TEST_CASE("utils::Occupancy: : index bijection1D", "occupancy_map")
{
  using Index1D = std::array<unsigned, 1>;
  const auto dims = Index1D{ 7 };

  for (std::size_t i = 0; i < 7; i++) {
    REQUIRE(utils::compute_index<Index1D, Index1D>(
              utils::compute_nd_index<Index1D, Index1D>(i, dims), dims) == i);
  }
}

TEST_CASE("utils::Occupancy: : index bijection2D", "occupancy_map")
{
  using Index2D = std::array<unsigned, 2>;
  const auto dims = Index2D{ 7, 3 };

  for (std::size_t i = 0; i < 7 * 3; i++) {
    REQUIRE(utils::compute_index<Index2D, Index2D>(
              utils::compute_nd_index<Index2D, Index2D>(i, dims), dims) == i);
  }
}
