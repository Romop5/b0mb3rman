#include <bm/navigation_mesh.hpp>
#include <unordered_set>

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

  std::unordered_map<WorldPosition, unsigned> position_to_parition_id;

  using WorldPosition = glm::vec2;
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
}