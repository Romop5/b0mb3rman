#include <catch2/catch_test_macros.hpp>

#include <utils/graph.hpp>
#include <utils/graph_algorithms.hpp>

namespace {
template<typename Map>
auto
are_first_keys_in_second_map(Map first, Map second) -> bool
{
  for (const auto& key : first) {
    if (second.count(key) == 0)
      return false;
  }
  return true;
}

template<typename Map>
auto
are_equal(Map first, Map second) -> bool
{
  return (are_first_keys_in_second_map(first, second) and
          are_first_keys_in_second_map(second, first));
}
} // namespace

TEST_CASE("utils::GraphAlgorithms: : has_circle", "graph")
{
  utils::UnorientedGraph<> graph;
  graph.add_edge(0, 1);
  REQUIRE_FALSE(utils::graph_algorithms::has_circle(graph));
  graph.add_edge(1, 2);
  REQUIRE_FALSE(utils::graph_algorithms::has_circle(graph));
  graph.add_edge(2, 0);
  REQUIRE(utils::graph_algorithms::has_circle(graph));
}

TEST_CASE("utils::GraphAlgorithms: : make_reflexive", "graph")
{
  utils::UnorientedGraph<> graph;
  graph = utils::graph_algorithms::make_reflexive(graph);
  REQUIRE(graph.empty_vertices());
  REQUIRE(graph.empty_edges());

  graph.add_vertex(0);
  REQUIRE_FALSE(graph.has_edge(0, 0));
  graph = utils::graph_algorithms::make_reflexive(graph);
  REQUIRE(graph.has_edge(0, 0));
}

TEST_CASE("utils::GraphAlgorithms: : make_symmetric", "graph")
{
  utils::OrientedGraph<> graph;
  graph = utils::graph_algorithms::make_symmetric(graph);
  REQUIRE(graph.empty_vertices());
  REQUIRE(graph.empty_edges());

  graph.add_edge(0, 1);
  REQUIRE_FALSE(graph.has_edge(1, 0));
  graph = utils::graph_algorithms::make_symmetric(graph);
  REQUIRE(graph.has_edge(1, 0));
}

TEST_CASE("utils::GraphAlgorithms: : make_transitive", "graph")
{
  utils::OrientedGraph<> graph;
  graph = utils::graph_algorithms::make_transitive(graph);
  REQUIRE(graph.empty_vertices());
  REQUIRE(graph.empty_edges());

  graph.add_edge(0, 1);
  graph.add_edge(1, 0);
  REQUIRE_FALSE(graph.has_edge(0, 0));
  graph = utils::graph_algorithms::make_transitive(graph);
  REQUIRE(graph.has_edge(0, 0));
}

TEST_CASE("utils::GraphAlgorithms: : compute_strong_components - trivial",
          "graph")
{
  utils::UnorientedGraph<> graph({ 0, 1, 2, 3 }, {});
  REQUIRE(are_equal(graph.get_neighbours(0), {}));
  REQUIRE(are_equal(graph.get_neighbours(1), {}));
  REQUIRE(are_equal(graph.get_neighbours(2), {}));
  REQUIRE(are_equal(graph.get_neighbours(3), {}));
  graph = utils::graph_algorithms::make_strong_components(graph);
  REQUIRE(are_equal(graph.get_neighbours(0), { 0 }));
  REQUIRE(are_equal(graph.get_neighbours(1), { 1 }));
  REQUIRE(are_equal(graph.get_neighbours(2), { 2 }));
  REQUIRE(are_equal(graph.get_neighbours(3), { 3 }));
}

TEST_CASE("utils::GraphAlgorithms: : compute_strong_components", "graph")
{
  utils::UnorientedGraph<> graph({ 0, 1, 2, 3 },
                                 {
                                   {
                                     { 0, 1 },
                                     { 1, 2 },
                                     { 2, 0 },
                                   },
                                 });
  REQUIRE(are_equal(graph.get_neighbours(0), { 1, 2 }));
  REQUIRE(are_equal(graph.get_neighbours(1), { 0, 2 }));
  REQUIRE(are_equal(graph.get_neighbours(2), { 0, 1 }));
  REQUIRE(are_equal(graph.get_neighbours(3), {}));
  graph = utils::graph_algorithms::make_strong_components(graph);
  REQUIRE(are_equal(graph.get_neighbours(0), { 0, 1, 2 }));
  REQUIRE(are_equal(graph.get_neighbours(1), { 0, 1, 2 }));
  REQUIRE(are_equal(graph.get_neighbours(2), { 0, 1, 2 }));
  REQUIRE(are_equal(graph.get_neighbours(3), { 3 }));
}

TEST_CASE("utils::GraphAlgorithms: : compute_path", "graph")
{
  utils::UnorientedGraph<> graph({ 0, 1, 2, 3 },
                                 {
                                   {
                                     { 0, 0 },
                                     { 0, 1 },
                                     { 1, 2 },
                                     { 2, 0 },
                                   },
                                 });

  {
    auto path = utils::graph_algorithms::compute_path(graph, 0, 0);
    REQUIRE(not path.empty());
    REQUIRE(path.size() == 1);
  }
  {
    auto path = utils::graph_algorithms::compute_path(graph, 0, 1);
    REQUIRE(not path.empty());
  }
  {
    auto path = utils::graph_algorithms::compute_path(graph, 0, 3);
    REQUIRE(path.empty());
  }

  {
    auto path = utils::graph_algorithms::compute_path(graph, 0, 2);
    REQUIRE(not path.empty());
  }

  {
    auto path = utils::graph_algorithms::compute_shortest_path(graph, 0, 0);
    REQUIRE(not path.empty());
    REQUIRE(path.size() == 1);
    REQUIRE(path.at(0) == 0);
  }

  {
    auto path = utils::graph_algorithms::compute_shortest_path(graph, 0, 2);
    REQUIRE(not path.empty());
    REQUIRE(path.size() == 2);
    REQUIRE(path.at(0) == 0);
    REQUIRE(path.at(1) == 2);
  }

  {
    auto path = utils::graph_algorithms::compute_shortest_path(graph, 0, 3);
    REQUIRE(path.empty());
  }

  {
    auto path = utils::graph_algorithms::compute_shortest_path(graph, 0, 1);
    REQUIRE(not path.empty());
    REQUIRE(path.size() == 2);
    REQUIRE(path.at(0) == 0);
    REQUIRE(path.at(1) == 1);
  }
  {
    auto path = utils::graph_algorithms::compute_shortest_path(graph, 0, 3);
    REQUIRE(path.empty());
  }
}
