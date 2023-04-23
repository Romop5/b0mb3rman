#include <cassert>
#include <iostream>

#include "occupancy_map.hpp"

int
main()
{
  utils::OccupancyMap<2, bool> map({ 1, 1 }, false);

  assert(map.at({ 0, 0 }) == false);
  map.at({ 0, 0 }) = true;
  assert(map.at({ 0, 0 }) == true);

  utils::OccupancyMap<2, bool>::Index index{ 3, 2 };
  utils::OccupancyMap<2, bool>::Dims dims{ 5, 5 };
  std::cout << utils::compute_index(index, dims) << std::endl;
}