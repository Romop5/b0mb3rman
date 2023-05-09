#include <bm/navigation_mesh.hpp>
#include <unordered_set>
#include <utils/occupancy_map.hpp>

using namespace bm;
NavigationMesh::NavigationMesh(bm::interfaces::ICollisionWorld& world)
  : world_{ world }
{
}

auto
NavigationMesh::update() -> void
{
  const auto boundary = world_.get_world_boundaries();
  const auto size = boundary.get_size();
  const auto origin = boundary.get_size();

  auto occupancy_map = utils::OccupancyMap2D<bool>(
    { static_cast<unsigned>(size.x), static_cast<unsigned>(size.y) }, false);

  occupancy_map.for_each([&](const auto index, auto is_occupied) {
    is_occupied =
      world_.has_static_collision(glm::vec2(index.at(0), index.at(1)));
  });

  /*  using WorldPosition = glm::ivec2;

    std::unordered_map<WorldPosition, unsigned> position_to_parition_id;

    // Generate: given world boundary, compute all possible world positions
    std::unordered_set<WorldPosition> all_possible_positions;
    for (unsigned i = 0; i < size.x; i++) {
      for (unsigned j = 0; i < size.y; j++) {
        existing_positions.insert(std::make_pair(origin + glm::vec2(i, j)));
      }
    }

    // Transform: keep collision-less positions to create a set of initial
    // positions
    std::unordered_set<WorldPosition> collision_less_positions;
    std::copy_if(all_possible_positions.begin(),
                 all_possible_positions.end(),
                 std::back_insert_iterator(collision_less_positions),
                 [&](const auto position) { world_.has_static_collision() });

    // Initialize a set of potential partitions to a set of all collision-less
    // positions
    auto remaning_positions_to_evaluate = collision_less_positions;
  */
}

auto
NavigationMesh::is_reachable(glm::vec2 start, glm::vec2 end) -> bool
{
  return false;
}

auto
NavigationMesh::compute_path(glm::vec2 start, glm::vec2 end)
  -> std::vector<glm::vec2>
{
  return {};
}