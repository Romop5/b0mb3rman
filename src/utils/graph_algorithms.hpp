#pragma once

#include <stack>
#include <unordered_set>

namespace utils {
namespace graph_algorithms {

/**
 * @brief Detects circle (symmetric-graph only)
 *
 * @tparam G
 * @param graph
 * @return true
 * @return false
 */
template<typename G>
auto
has_circle(const G& graph) -> bool
{
  if (graph.empty_edges() or graph.empty_vertices()) {
    return false;
  }

  std::unordered_set<typename G::NodeId> visited_vertices;

  // Proof:
  // - initiate set V of visited vertices
  // 1. empty graph G is circle-less (trivial)
  // 2. Until exhaustion of a set of edges:
  //    Consider graph G. If by adding an edge E = (v1,v2) we connect two
  //    already visited vertices, than extended graph G' (G with the new edge)
  //    contains a circle.
  //
  //    Otherwise, it does not, and we add 'v1' and 'v2' to
  //    the set of visited vertices and continue.
  // Complexity: O(e) (linear with edges)

  for (const auto& [edge, _] : graph.get_edges()) {
    if (visited_vertices.count(edge.first) > 0 and
        visited_vertices.count(edge.second) > 0) {
      return true;
    } else {
      visited_vertices.insert(edge.first);
      visited_vertices.insert(edge.second);
    }
  }
  return false;
}

template<typename G>
auto
make_reflexive(const G& graph) -> G
{
  // Complexity: O(v+e)
  auto result = graph;
  for (const auto& vertex : result.get_vertices()) {
    result.add_edge(vertex, vertex);
  }

  for (const auto& [edge, _] : result.get_edges()) {
    result.add_edge(edge.first, edge.first);
    result.add_edge(edge.second, edge.second);
  }
  return result;
}

template<typename G>
auto
make_symmetric(const G& graph) -> G
{
  auto result = graph;

  // Complexity: O(e)
  for (const auto& [edge, _] : result.get_edges()) {
    result.add_edge(edge.second, edge.first);
  }
  return result;
}

template<typename G>
auto
make_transitive(const G& graph) -> G
{
  auto result = graph;

  bool was_updated = false;
  do {
    was_updated = false;
    for (const auto& [edge, _] : result.get_edges()) {
      for (const auto& transitive : graph.get_neighbours(edge.second)) {
        if (not result.has_edge(edge.first, transitive)) {
          result.add_edge(edge.first, transitive);
          was_updated = true;
        }
      }
    }
  } while (was_updated);
  return result;
}

} // namespace utils::graph_algorihms
} // namespace utils