#include <catch2/catch_test_macros.hpp>

#include <utils/graph.hpp>
#include <utils/graph_algorithms.hpp>

TEST_CASE("utils::GraphAlgorithms: : has_circle", "graph")
{
  utils::SymmetricGraph<> graph;
  graph.add_edge(0, 1);
  REQUIRE_FALSE(utils::graph_algorithms::has_circle(graph));
  graph.add_edge(1, 2);
  REQUIRE_FALSE(utils::graph_algorithms::has_circle(graph));
  graph.add_edge(2, 0);
  REQUIRE(utils::graph_algorithms::has_circle(graph));
}

TEST_CASE("utils::GraphAlgorithms: : make_reflexive", "graph")
{
  utils::SymmetricGraph<> graph;
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