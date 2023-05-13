#include <catch2/catch_test_macros.hpp>

#include <bm/navigation_mesh.hpp>

namespace {

using namespace bm;
class CollisionWorldMock : public bm::interfaces::ICollisionWorld
{
public:
  CollisionWorldMock(std::vector<bool> collision_map, glm::vec2 size)
    : collision_map_{ collision_map }
    , boundary_{ glm::vec2(0, 0), size }
  {
  }
  auto get_world_boundaries() -> utils::AABB override { return boundary_; }
  auto is_out_of_bounds(glm::vec2 position) -> bool override
  {
    return not boundary_.contains(position);
  }
  auto is_cell_occupied(glm::vec2 position, Entity::TypeMask allowed_types)
    -> bool override
  {
    return false;
  }
  auto has_static_collision(glm::vec2 position) -> bool override
  {
    const auto index = static_cast<unsigned>(position.x) +
                       static_cast<unsigned>(position.y * boundary_.size_.x);
    return collision_map_.at(index);
  }

private:
  std::vector<bool> collision_map_;
  utils::AABB boundary_;
};
}

TEST_CASE("utils::NavigationMesh: constructor", "navigation_mesh")
{
  using namespace utils::detail;

  //  Occupancy (middle row is occupied)
  //   ┌───┐
  //   │.x.│
  //   │.x.│
  //   │.x.│
  //   └───┘
  //
  const auto collision_map = std::vector<bool>{
    false, true, false, false, true, false, false, true, false,
  };
  auto mocked_world = CollisionWorldMock(collision_map, glm::vec2(3, 3));
  bm::NavigationMesh navmesh(mocked_world);
  navmesh.update();

  // Identity: the same place should always be reachable
  REQUIRE(navmesh.is_reachable(glm::vec2(0, 0), glm::vec2(0, 0)));
  // The same partition (the same row)
  REQUIRE(navmesh.is_reachable(glm::vec2(0, 0), glm::vec2(0, 1)));
  REQUIRE(navmesh.is_reachable(glm::vec2(0, 0), glm::vec2(0, 2)));

  REQUIRE(navmesh.compute_path(glm::vec2(0, 0), glm::vec2(0, 0)).size() == 1);
  REQUIRE(navmesh.compute_path(glm::vec2(0, 0), glm::vec2(0, 1)).size() == 2);
  REQUIRE(navmesh.compute_path(glm::vec2(0, 0), glm::vec2(0, 2)).size() == 3);

  REQUIRE_FALSE(navmesh.is_reachable(glm::vec2(0, 0), glm::vec2(1, 0)));
  REQUIRE_FALSE(navmesh.is_reachable(glm::vec2(0, 0), glm::vec2(1, 1)));
  REQUIRE_FALSE(navmesh.is_reachable(glm::vec2(0, 0), glm::vec2(1, 2)));

  REQUIRE(navmesh.compute_path(glm::vec2(0, 0), glm::vec2(1, 0)).size() == 0);
  REQUIRE(navmesh.compute_path(glm::vec2(0, 0), glm::vec2(1, 1)).size() == 0);
  REQUIRE(navmesh.compute_path(glm::vec2(0, 0), glm::vec2(1, 2)).size() == 0);

  // Partitions differ (two rows are separated by the middle row)
  REQUIRE_FALSE(navmesh.is_reachable(glm::vec2(0, 0), glm::vec2(2, 0)));
  REQUIRE_FALSE(navmesh.is_reachable(glm::vec2(0, 0), glm::vec2(2, 1)));
  REQUIRE_FALSE(navmesh.is_reachable(glm::vec2(0, 0), glm::vec2(2, 2)));

  REQUIRE(navmesh.compute_path(glm::vec2(0, 0), glm::vec2(2, 0)).size() == 0);
  REQUIRE(navmesh.compute_path(glm::vec2(0, 0), glm::vec2(2, 1)).size() == 0);
  REQUIRE(navmesh.compute_path(glm::vec2(0, 0), glm::vec2(2, 2)).size() == 0);
}
