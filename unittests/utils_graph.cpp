#include <catch2/catch_test_macros.hpp>

#include <utils/graph.hpp>

TEST_CASE("utils::Graph: : basic", "graph")
{
  utils::OrientedGraph<> graph;
  REQUIRE(graph.empty_edges());

  REQUIRE(not graph.has_vertex(0));
  REQUIRE(not graph.has_vertex(1));
  graph.add_edge(0, 1);
  REQUIRE(graph.has_vertex(0));
  REQUIRE(graph.has_vertex(1));

  REQUIRE(graph.has_edge(0, 1));
  REQUIRE(not graph.has_edge(1, 0));
  REQUIRE(not graph.empty_edges());

  graph.remove_edge(0, 1);
  REQUIRE(graph.has_vertex(0));
  REQUIRE(graph.has_vertex(1));
  REQUIRE(graph.empty_edges());
}

TEST_CASE("utils::Graph: : basic symmetric graph", "graph")
{
  utils::SymmetricGraph<> graph;
  REQUIRE(graph.empty_edges());

  graph.add_edge(0, 1);
  REQUIRE(graph.has_edge(0, 1));
  REQUIRE(graph.has_edge(1, 0));
  REQUIRE(not graph.empty_edges());

  graph.remove_edge(1, 0);
  REQUIRE(graph.empty_edges());
}

TEST_CASE("utils::Graph: : graph with payload", "graph")
{
  utils::SymmetricGraph<unsigned> graph;
  REQUIRE(graph.empty_edges());

  graph.add_edge(0, 1, 100);
  REQUIRE(graph.has_edge(0, 1));
  REQUIRE(graph.has_edge(1, 0));
  REQUIRE(not graph.empty_edges());

  REQUIRE(graph.get_edge_data(0, 1) == 100);

  CHECK_THROWS(graph.get_edge_data(0, 0));
  graph.add_edge(0, 0, 666);
  CHECK_NOTHROW(graph.get_edge_data(0, 0));
  REQUIRE(graph.get_edge_data(0, 0) == 666);
}
