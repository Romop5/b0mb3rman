#include <bm/navigation_mesh.hpp>
#include <unordered_set>
#include <utils/graph_algorithms.hpp>
#include <utils/occupancy_map.hpp>

using namespace bm;
NavigationMesh::NavigationMesh(bm::interfaces::ICollisionWorld& world)
  : world_{ world }
{
}

auto
NavigationMesh::update() -> void
{
  // Invalidate cache
  cache_ = {};
  cache_.boundary = world_.get_world_boundaries();

  const auto size = cache_.boundary.get_size();
  const auto origin = cache_.boundary.get_size();

  auto occupancy_map = utils::OccupancyMap2D<bool>(
    { static_cast<unsigned>(size.x), static_cast<unsigned>(size.y) }, false);

  // Construct graph from non-colliding vertices
  occupancy_map.for_each([&](const auto index, auto is_occupied) {
    is_occupied =
      world_.has_static_collision(glm::vec2(index.at(0), index.at(1)));

    if (not is_occupied) {
      cache_.graph.add_vertex(occupancy_map.transform_index_to_offset(index));
    }
  });

  // Connect neighbours in the graph
  for (size_t col = 0; col < size.x - 1; col++) {
    for (size_t row = 0; row < size.y - 1; row++) {
      const auto node_id = compute_node_id({ col, row });

      std::vector<utils::UnorientedGraph<>::NodeId> possible_neighbours = {
        compute_node_id({ col, row }),
        compute_node_id({ col + 1, row }),
        compute_node_id({ col, row + 1 })
      };

      for (const auto& neighbour_id : possible_neighbours) {
        if (cache_.graph.has_vertex(neighbour_id)) {
          cache_.graph.add_edge(node_id, neighbour_id);
        }
      }
    }
  }

  // Compute strong components from graph
  cache_.strong_components =
    utils::graph_algorithms::make_strong_components(cache_.graph);
}

auto
NavigationMesh::is_reachable(glm::vec2 start, glm::vec2 end) -> bool
{
  return not compute_path(start, end).empty();
}

auto
NavigationMesh::compute_path(glm::vec2 start, glm::vec2 end)
  -> std::vector<glm::vec2>
{
  auto node_id_paths = utils::graph_algorithms::compute_shortest_path(
    cache_.graph, compute_node_id(start), compute_node_id(end));

  std::vector<glm::vec2> result(node_id_paths.size());
  std::transform(node_id_paths.begin(),
                 node_id_paths.end(),
                 result.begin(),
                 [this](const auto node_id) {
                   return compute_position_from_node_id(node_id);
                 });
  return result;
}

auto
NavigationMesh::compute_node_id(glm::vec2 position)
  -> utils::UnorientedGraph<>::NodeId
{
  if (not cache_.boundary.contains(position)) {
    throw std::runtime_error("compute_node_id: Position of bounds");
  }

  return utils::compute_index(
    std::array<unsigned long, 2>{ position.x, position.y },
    std::array<unsigned long, 2>{ cache_.boundary.size_.x,
                                  cache_.boundary.size_.y });
}

auto
NavigationMesh::compute_position_from_node_id(
  utils::UnorientedGraph<>::NodeId id) -> glm::vec2
{
  const auto array_index =
    utils::compute_nd_index<std::array<unsigned long, 2>>(
      id,
      std::array<unsigned long, 2>{ cache_.boundary.size_.x,
                                    cache_.boundary.size_.y });
  return { array_index[0], array_index[1] };
}