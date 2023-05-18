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
  utils::UnorientedGraph<> graph;
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
  utils::UnorientedGraph<unsigned> graph;
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

TEST_CASE("utils::Graph: unoriented graph", "graph")
{
  utils::UnorientedGraph<> graph;
  graph.add_vertex(0);
  graph.add_vertex(1);
  graph.add_vertex(2);
  graph.add_edge(0, 1);

  REQUIRE(graph.has_edge(0, 1));
  REQUIRE(graph.has_edge(1, 0));
  REQUIRE_FALSE(graph.has_edge(0, 2));
  REQUIRE_FALSE(graph.has_edge(2, 0));
  REQUIRE_FALSE(graph.has_edge(1, 2));
  REQUIRE_FALSE(graph.has_edge(2, 1));

  REQUIRE_FALSE(graph.get_neighbours(0).count(0));
  REQUIRE_FALSE(graph.get_neighbours(0).count(2));
  REQUIRE_FALSE(graph.get_neighbours(0).count(3));
  REQUIRE(graph.get_neighbours(0).count(1));
  REQUIRE(graph.get_neighbours(1).count(0));
  REQUIRE_FALSE(graph.get_neighbours(1).count(1));
  REQUIRE_FALSE(graph.get_neighbours(1).count(2));

  REQUIRE_FALSE(graph.get_neighbours(2).count(0));
  REQUIRE_FALSE(graph.get_neighbours(2).count(1));
  REQUIRE_FALSE(graph.get_neighbours(2).count(2));
}

TEST_CASE("utils::Graph: oriented graph", "graph")
{
  utils::OrientedGraph<> graph;
  graph.add_vertex(0);
  graph.add_vertex(1);
  graph.add_vertex(2);
  graph.add_edge(0, 1);

  REQUIRE(graph.has_edge(0, 1));
  REQUIRE_FALSE(graph.has_edge(0, 2));
  REQUIRE_FALSE(graph.has_edge(2, 0));
  REQUIRE_FALSE(graph.has_edge(1, 0));
  REQUIRE_FALSE(graph.has_edge(1, 2));
  REQUIRE_FALSE(graph.has_edge(2, 1));

  REQUIRE_FALSE(graph.get_neighbours(0).count(0));
  REQUIRE_FALSE(graph.get_neighbours(0).count(2));
  REQUIRE_FALSE(graph.get_neighbours(0).count(3));
  REQUIRE(graph.get_neighbours(0).count(1));
  REQUIRE_FALSE(graph.get_neighbours(1).count(0));
  REQUIRE_FALSE(graph.get_neighbours(1).count(1));
  REQUIRE_FALSE(graph.get_neighbours(1).count(2));

  REQUIRE_FALSE(graph.get_neighbours(2).count(0));
  REQUIRE_FALSE(graph.get_neighbours(2).count(1));
  REQUIRE_FALSE(graph.get_neighbours(2).count(2));
}
