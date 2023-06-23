#pragma once

#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant>

namespace utils {
namespace detail {
struct pair_hash
{
  template<class T1, class T2>
  std::size_t operator()(const std::pair<T1, T2>& pair) const
  {
    return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
  }
};

template<class T>
auto
order_pair(const std::pair<T, T> pair) -> std::pair<T, T>
{
  return pair.first < pair.second ? pair
                                  : std::make_pair(pair.second, pair.first);
}

} // namespace utils::detail

enum GraphOrientation
{
  unoriented = 0,
  oriented = 1
};

/**
 * @brief Sparse graph with optional storage (per-edge)
 *
 * @note implemented via hash map
 *
 */
template<typename _EdgeStorage = std::monostate,
         unsigned _Orientation = GraphOrientation::unoriented>
class Graph
{
public:
  using EdgeStorage = _EdgeStorage;
  static constexpr auto Orientation = _Orientation;
  using NodeId = unsigned;
  using Edge = std::pair<NodeId, NodeId>;

  Graph() = default;
  Graph(std::unordered_set<NodeId> vertices,
        std::unordered_set<Edge, detail::pair_hash> edges);

  auto has_edge(NodeId a, NodeId b) const -> bool;
  auto get_edge_data(NodeId a, NodeId b) const -> EdgeStorage;

  auto empty_edges() const -> bool;
  auto empty_vertices() const -> bool;

  auto add_vertex(NodeId vertex) -> void;
  auto has_vertex(NodeId vertex) const -> bool;
  auto remove_vertex(NodeId vertex) -> void;
  auto get_neighbours(NodeId vertex) const -> std::unordered_set<NodeId>;

  auto add_edge(NodeId a, NodeId b, EdgeStorage data = {}) -> void;
  auto remove_edge(NodeId a, NodeId b) -> void;

  auto get_vertices() const -> const std::unordered_set<NodeId>&;
  auto get_edges() const
    -> const std::unordered_map<Edge, EdgeStorage, detail::pair_hash>&;

private:
  auto transform_pair(NodeId a, NodeId b) const -> std::pair<NodeId, NodeId>;

private:
  std::unordered_set<NodeId> vertices_;
  std::unordered_map<Edge, EdgeStorage, detail::pair_hash> edges_;
};

template<typename EdgeStorage = std::monostate>
using UnorientedGraph = Graph<EdgeStorage, GraphOrientation::unoriented>;

template<typename EdgeStorage = std::monostate>
using OrientedGraph = Graph<EdgeStorage, GraphOrientation::oriented>;

template<typename EdgeStorage, unsigned Orientation>
Graph<EdgeStorage, Orientation>::Graph(
  std::unordered_set<Graph::NodeId> vertices,
  std::unordered_set<Graph::Edge, detail::pair_hash> edges)
  : vertices_{ vertices }
{
  for (const auto& edge : edges) {
    edges_[edge] = {};
  }
}

template<typename EdgeStorage, unsigned Orientation>
auto
Graph<EdgeStorage, Orientation>::has_edge(NodeId a, NodeId b) const -> bool
{
  return edges_.count(transform_pair(a, b)) > 0;
}

template<typename EdgeStorage, unsigned Orientation>
auto
Graph<EdgeStorage, Orientation>::get_edge_data(NodeId a, NodeId b) const
  -> EdgeStorage
{
  if constexpr (std::is_same<std::monostate, EdgeStorage>::value) {
    return {};
  }

  const auto p = transform_pair(a, b);
  return edges_.at(p);
}

template<typename EdgeStorage, unsigned Orientation>
auto
Graph<EdgeStorage, Orientation>::empty_edges() const -> bool
{
  return edges_.empty();
}

template<typename EdgeStorage, unsigned Orientation>
auto
Graph<EdgeStorage, Orientation>::empty_vertices() const -> bool
{
  return vertices_.empty();
}

template<typename EdgeStorage, unsigned Orientation>
auto
Graph<EdgeStorage, Orientation>::add_vertex(NodeId vertex) -> void
{
  vertices_.insert(vertex);
}

template<typename EdgeStorage, unsigned Orientation>
auto
Graph<EdgeStorage, Orientation>::has_vertex(NodeId vertex) const -> bool
{
  return vertices_.count(vertex);
}

template<typename EdgeStorage, unsigned Orientation>
auto
Graph<EdgeStorage, Orientation>::remove_vertex(NodeId vertex) -> void
{
  vertices_.erase(vertex);

  // Implicitely delete all connected edges
  // TODO:
  for (const auto neighbour : get_neighbours()) {
    remove_edge(vertex, neighbour);
  }
}

template<typename EdgeStorage, unsigned Orientation>
auto
Graph<EdgeStorage, Orientation>::get_neighbours(NodeId vertex) const
  -> std::unordered_set<NodeId>
{
  std::unordered_set<NodeId> result;
  for (auto& [edge, _] : edges_) {
    const auto pair = transform_pair(edge.first, edge.second);

    if (pair.first != vertex and pair.second != vertex) {
      continue;
    }

    if constexpr (Orientation == GraphOrientation::oriented) {
      if (pair.first != vertex) {
        continue;
      }
      const auto neighbour = pair.second;
      result.insert(neighbour);
    } else {
      const auto neighbour = pair.first == vertex ? pair.second : pair.first;
      result.insert(neighbour);
    }
  }
  return result;
}

template<typename EdgeStorage, unsigned Orientation>
auto
Graph<EdgeStorage, Orientation>::add_edge(NodeId a, NodeId b, EdgeStorage data)
  -> void
{
  edges_[transform_pair(a, b)] = data;

  // Implicitely add vertices
  add_vertex(a);
  add_vertex(b);
}

template<typename EdgeStorage, unsigned Orientation>
auto
Graph<EdgeStorage, Orientation>::remove_edge(NodeId a, NodeId b) -> void
{
  edges_.erase(transform_pair(a, b));
}

template<typename EdgeStorage, unsigned Orientation>
auto
Graph<EdgeStorage, Orientation>::transform_pair(NodeId a, NodeId b) const
  -> std::pair<NodeId, NodeId>
{
  if constexpr (Orientation == GraphOrientation::unoriented) {
    return detail::order_pair(std::make_pair(a, b));
  } else {
    return { a, b };
  }
}
template<typename EdgeStorage, unsigned Orientation>
auto
Graph<EdgeStorage, Orientation>::get_vertices() const
  -> const std::unordered_set<NodeId>&
{
  return vertices_;
}

template<typename EdgeStorage, unsigned Orientation>
auto
Graph<EdgeStorage, Orientation>::get_edges() const
  -> const std::unordered_map<Edge, EdgeStorage, detail::pair_hash>&
{
  return edges_;
}

} // namespace utils