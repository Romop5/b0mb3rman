#pragma once

#include <queue>
#include <unordered_set>

namespace utils {
namespace graph_algorithms {

template<typename G, typename NodeId = typename G::NodeId>
auto
reachable_nodes(const G& graph, NodeId start) -> std::unordered_set<NodeId>
{
  std::unordered_set<NodeId> visited_vertices;
  std::queue<NodeId> remaining_vertices;

  // Start with start node
  remaining_vertices.push(start);

  // while not exhausted a set of all reachable vertices (from start)
  while (not remaining_vertices.empty()) {
    const auto vertex = remaining_vertices.front();
    remaining_vertices.pop();

    // Skip already visited vertex
    if (visited_vertices.count(vertex) > 0) {
      continue;
    }
    visited_vertices.insert(vertex);

    // append neighbours as next reachable vertices
    for (const auto& next : graph.get_neighbours(vertex)) {
      remaining_vertices.push(next);
    }
  }
  return visited_vertices;
}

/**
 * @brief Detect whether graph
 *
 * @tparam G both oriented/unoriented
 * @tparam G::NodeId
 * @param graph
 * @param start the initial node
 * @return std::unordered_set<NodeId>
 */
template<typename G, typename NodeId = typename G::NodeId>
auto
has_circle(const G& graph, NodeId start) -> std::unordered_set<NodeId>
{
  return reachable_nodes(graph, start).count(start) > 0;
}

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
  using NodeId = typename G::NodeId;

  auto result = graph;
  std::vector<std::pair<NodeId, NodeId>> new_edges;
  do {
    for (const auto& edge : new_edges) {
      result.add_edge(edge.first, edge.second);
    }
    new_edges.clear();

    for (const auto& [edge, _] : result.get_edges()) {
      for (const auto transitive : graph.get_neighbours(edge.second)) {
        if (not result.has_edge(edge.first, transitive)) {
          new_edges.push_back({ edge.first, transitive });
        }
      }
    }

  } while (not new_edges.empty());
  return result;
}

/**
 * @brief Compute representatives of strong components of graph
 *
 * @tparam G symmetric graph
 * @param graph
 * @return std::vector<NodeId> representative nodes
 */
template<typename G, typename NodeId = typename G::NodeId>
auto
make_representatives_of_strong_components(const G& graph) -> std::vector<NodeId>
{
  std::vector<NodeId> representatives;

  // initially, all nodes are candidates
  std::unordered_set<NodeId> remaining_nodes = graph.get_vertices();

  // until exhaustion of nodes
  while (!remaining_nodes.empty()) {
    const auto representative_id = *remaining_nodes.begin();
    const auto reachable_nodes_set = reachable_nodes(graph, representative_id);

    // remove the nodes from the same component from set of potential
    // representatives
    for (const auto& reachable_node : reachable_nodes_set) {
      remaining_nodes.erase(reachable_node);
    }

    representatives.push_back(representative_id);
  }
  return representatives;
}

/**
 * @brief Computes partially-linear ordered set
 *
 * @tparam G
 * @param graph
 * @return G
 */
template<typename G, typename NodeId = typename G::NodeId>
auto
linearly_order_nodes(const G& graph) -> std::vector<NodeId>
{
  const auto transitive_reflexive_closure =
    make_transitive(make_reflexive(graph));
  const auto& trc = transitive_reflexive_closure;

  const auto reflexive_symmetric_closure =
    make_reflexive(make_symmetric(graph));
  const auto symmetric_transitive_reflexive_closure =
    make_transitive(make_reflexive(reflexive_symmetric_closure));
  const auto representatives =
    make_representatives_of_strong_components(reflexive_symmetric_closure);

  std::vector<NodeId> result;

  // for all components
  for (const auto& representative_id : representatives) {
    const auto component_set =
      symmetric_transitive_reflexive_closure.get_neighbours(representative_id);

    std::vector<NodeId> component(component_set.size());
    std::copy(component_set.begin(), component_set.end(), component.begin());

    // sort vertices within the strong component
    std::sort(component.begin(),
              component.end(),
              [&](const auto id_a, const auto id_b) {
                // trivial case
                if (id_a == id_b) {
                  return true;
                }
                // precondition: both id_a and id_b are the same strong
                // component
                assert(trc.has_edge(id_a, id_b) or trc.has_edge(id_b, id_a));
                if (trc.has_edge(id_a, id_b) && trc.has_edge(id_b, id_a)) {
                  throw std::runtime_error("Cycle detected");
                }
                return trc.has_edge(id_a, id_b);
              });
    // append linearized subcomponent to the rest
    std::copy(component.begin(), component.end(), std::back_inserter(result));
  }
  return result;
}

/**
 * @brief Naive implementation, but works for both types of graph
 * @note space complexity: O(e^2), may cause Out of Memory
 *
 * @tparam G
 * @param graph
 * @return G
 */
template<typename G>
auto
make_strong_components(const G& graph) -> G
{
  return make_transitive(make_reflexive(graph));
}

template<typename G, typename NodeId = typename G::NodeId>
auto
compute_path(const G& graph, NodeId start, NodeId end) -> std::vector<NodeId>
{
  std::unordered_set<NodeId> visited_vertices;
  std::unordered_map<NodeId, NodeId> previous_vertices;
  std::queue<NodeId> remaining_vertices;

  // Helper: constructs a path using pointers to previous vertices
  auto trace_path_back = [&](NodeId from) {
    std::vector<NodeId> result = { from };

    auto previous_vertex = from;
    while (previous_vertex != start) {
      previous_vertex = previous_vertices.at(previous_vertex);
      result.push_back(previous_vertex);
    }
    std::reverse(result.begin(), result.end());
    return result;
  };

  // Start with start node
  remaining_vertices.push(start);

  // while not exhausted a set of all reachable vertices (from start)
  while (not remaining_vertices.empty()) {
    const auto vertex = remaining_vertices.front();
    remaining_vertices.pop();

    // success: terminate with creating a vector of previous paths
    if (vertex == end) {
      return trace_path_back(vertex);
    }

    // Skip already visited vertex
    if (visited_vertices.count(vertex) > 0) {
      continue;
    }
    visited_vertices.insert(vertex);

    // append neighbours as next reachable vertices
    for (const auto& next : graph.get_neighbours(vertex)) {
      previous_vertices[next] = vertex;
      remaining_vertices.push(next);
    }
  }
  return {};
}

template<typename T, unsigned index>
struct CompareItem
{
  constexpr auto operator()(const T& lhs, const T& rhs) -> bool
  {
    return std::get<index>(lhs) > std::get<index>(rhs);
  }
};

template<typename G, typename NodeId = typename G::NodeId>
auto
compute_shortest_path(const G& graph, NodeId start, NodeId end)
  -> std::vector<NodeId>
{
  if (not graph.has_vertex(start) or not graph.has_vertex(end)) {
    return {};
  }
  /**
   * @brief Represents status of reachable node
   * @param first node
   * @param second distance from start
   * @param third previous node
   *
   */
  using ReachableNode = std::tuple<NodeId, unsigned, NodeId>;

  // Min. heap
  std::priority_queue<ReachableNode,
                      std::vector<ReachableNode>,
                      CompareItem<ReachableNode, 1>>
    remaining_vertices;

  std::unordered_map<NodeId, NodeId> previous_vertices;
  std::unordered_set<NodeId> visited_vertices;

  // Helper: constructs a path using pointers to previous vertices
  auto trace_path_back = [&](NodeId from) {
    std::vector<NodeId> result = { from };

    auto previous_vertex = from;
    while (previous_vertex != start) {
      previous_vertex = previous_vertices.at(previous_vertex);
      result.push_back(previous_vertex);
    }

    std::reverse(result.begin(), result.end());
    return result;
  };

  // Start with start node
  remaining_vertices.push({ start, 0, start });

  // while not exhausted a set of all reachable vertices (from start)
  while (not remaining_vertices.empty()) {
    const auto [vertex, distance, previous_vertex] = remaining_vertices.top();
    remaining_vertices.pop();

    // Skip already visited vertex (prevents cycling when path does not exist)
    if (visited_vertices.count(vertex) > 0) {
      continue;
    }
    visited_vertices.insert(vertex);
    previous_vertices[vertex] = previous_vertex;

    // success: terminate with creating a vector of previous paths
    if (vertex == end) {
      return trace_path_back(vertex);
    }

    // append neighbours as next reachable vertices
    for (const auto& next : graph.get_neighbours(vertex)) {
      remaining_vertices.push({ next, distance + 1, vertex });
    }
  }
  return {};
}

} // namespace utils::graph_algorihms
} // namespace utils