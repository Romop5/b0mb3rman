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

TEST_CASE("utils::GraphAlgorithms: : compute_strong_components - empty",
          "graph")
{
  utils::UnorientedGraph<> graph;
  graph = utils::graph_algorithms::make_strong_components(graph);
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

TEST_CASE("utils::GraphAlgorithms: : compute_strong_components_larger", "graph")
{
  utils::UnorientedGraph<> graph;
  graph.add_edge(0, 1);
  graph.add_edge(1, 0);
  graph.add_edge(1, 2);
  graph.add_edge(2, 1);
  graph.add_edge(3, 4);
  graph.add_edge(4, 3);
  graph.add_edge(4, 5);
  graph.add_edge(5, 6);
  graph.add_edge(10, 11);

  graph = utils::graph_algorithms::make_strong_components(graph);
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

TEST_CASE("utils::GraphAlgorithms: : strong components representative", "graph")
{
  utils::OrientedGraph<> graph;
  graph.add_edge(0, 1);
  const auto reflexive_symmetric_closure =
    utils::graph_algorithms::make_reflexive(
      utils::graph_algorithms::make_symmetric(graph));

  const auto representatives =
    utils::graph_algorithms::make_representatives_of_strong_components(
      reflexive_symmetric_closure);

  REQUIRE(representatives.size() == 1);
  REQUIRE((representatives.at(0) == 0 or representatives.at(0) == 1));
}

TEST_CASE("utils::GraphAlgorithms: : strong components representative 2",
          "graph")
{
  utils::OrientedGraph<> graph;
  graph.add_vertex(0);
  graph.add_vertex(1);

  const auto reflexive_symmetric_closure =
    utils::graph_algorithms::make_reflexive(
      utils::graph_algorithms::make_symmetric(graph));

  const auto representatives =
    utils::graph_algorithms::make_representatives_of_strong_components(
      reflexive_symmetric_closure);

  REQUIRE(representatives.size() == 2);
  REQUIRE((representatives.at(0) == 0 or representatives.at(0) == 1));
  REQUIRE((representatives.at(1) == 0 or representatives.at(1) == 1));
  REQUIRE(representatives.at(0) != representatives.at(1));
}

TEST_CASE("utils::GraphAlgorithms: : strong components representative 3",
          "graph")
{
  utils::OrientedGraph<> graph;
  graph.add_vertex(0);
  graph.add_vertex(1);
  graph.add_vertex(2);
  graph.add_edge(0, 1);

  const auto reflexive_symmetric_closure =
    utils::graph_algorithms::make_reflexive(
      utils::graph_algorithms::make_symmetric(graph));

  const auto representatives =
    utils::graph_algorithms::make_representatives_of_strong_components(
      reflexive_symmetric_closure);

  REQUIRE(representatives.size() == 2);
  REQUIRE((representatives.at(0) == 0 or representatives.at(0) == 1 or
           representatives.at(0) == 2));
  REQUIRE((representatives.at(1) == 0 or representatives.at(1) == 1 or
           representatives.at(1) == 2));
  REQUIRE(representatives.at(0) != representatives.at(1));
}

TEST_CASE("utils::GraphAlgorithms: reachable nodes", "graph")
{
  utils::OrientedGraph<> graph;
  graph.add_edge(0, 1);
  graph.add_edge(1, 2);
  graph.add_vertex(3);

  {
    const auto nodes = utils::graph_algorithms::reachable_nodes(graph, 0);
    REQUIRE(nodes.count(0) > 0);
    REQUIRE(nodes.count(1) > 0);
    REQUIRE(nodes.count(2) > 0);
    REQUIRE(nodes.count(3) == 0);
  }

  {
    const auto nodes = utils::graph_algorithms::reachable_nodes(graph, 1);
    REQUIRE(nodes.count(0) == 0);
    REQUIRE(nodes.count(1) > 0);
    REQUIRE(nodes.count(2) > 0);
    REQUIRE(nodes.count(3) == 0);
  }

  {
    const auto nodes = utils::graph_algorithms::reachable_nodes(graph, 2);
    REQUIRE(nodes.count(0) == 0);
    REQUIRE(nodes.count(1) == 0);
    REQUIRE(nodes.count(2) > 0);
    REQUIRE(nodes.count(3) == 0);
  }

  {
    const auto nodes = utils::graph_algorithms::reachable_nodes(graph, 3);
    REQUIRE(nodes.count(0) == 0);
    REQUIRE(nodes.count(1) == 0);
    REQUIRE(nodes.count(2) == 0);
    REQUIRE(nodes.count(3) > 0);
  }
}

TEST_CASE("utils::GraphAlgorithms: : linearize_set empty", "graph")
{
  utils::OrientedGraph<> graph;

  const auto order = utils::graph_algorithms::linearly_order_nodes(graph);
}

TEST_CASE("utils::GraphAlgorithms: : linearize_set", "graph")
{
  utils::OrientedGraph<> graph;
  graph.add_edge(0, 1);

  const auto order = utils::graph_algorithms::linearly_order_nodes(graph);
  REQUIRE(order.at(0) == 0);
  REQUIRE(order.at(1) == 1);
}

TEST_CASE("utils::GraphAlgorithms: : linearize_set detect cycle", "graph")
{
  utils::OrientedGraph<> graph;
  graph.add_edge(0, 1);
  graph.add_edge(1, 0);

  CHECK_THROWS(utils::graph_algorithms::linearly_order_nodes(graph));
}

TEST_CASE("utils::GraphAlgorithms: : linearize_set detect cycle advanced",
          "graph")
{
  utils::OrientedGraph<> graph;
  graph.add_edge(0, 1);
  graph.add_edge(1, 10);
  graph.add_edge(10, 2);
  graph.add_edge(2, 1);

  CHECK_THROWS(utils::graph_algorithms::linearly_order_nodes(graph));
}

TEST_CASE("utils::GraphAlgorithms: : linearize_set detect cycle (unoriented)",
          "graph")
{
  utils::UnorientedGraph<> graph;
  graph.add_edge(0, 1);

  CHECK_THROWS(utils::graph_algorithms::linearly_order_nodes(graph));
}

TEST_CASE("utils::GraphAlgorithms: : linearize_set ok", "graph")
{
  utils::OrientedGraph<> graph;
  graph.add_vertex(0);
  graph.add_vertex(1);

  const auto order = utils::graph_algorithms::linearly_order_nodes(graph);
  REQUIRE((order.at(0) == 0 or order.at(0) == 1));
  REQUIRE((order.at(1) == 0 or order.at(1) == 1));
  REQUIRE(order.at(0) != order.at(1));
}

TEST_CASE("utils::GraphAlgorithms: : linearize_set first 3", "graph")
{
  utils::OrientedGraph<> graph;
  graph.add_edge(0, 1);
  graph.add_edge(1, 2);

  const auto order = utils::graph_algorithms::linearly_order_nodes(graph);
  REQUIRE(order.at(0) == 0);
  REQUIRE(order.at(1) == 1);
  REQUIRE(order.at(2) == 2);
}

TEST_CASE("utils::GraphAlgorithms: : linearize_set first 3 reversed", "graph")
{
  utils::OrientedGraph<> graph;
  graph.add_edge(2, 1);
  graph.add_edge(1, 0);

  const auto order = utils::graph_algorithms::linearly_order_nodes(graph);
  REQUIRE(order.at(2) == 0);
  REQUIRE(order.at(1) == 1);
  REQUIRE(order.at(0) == 2);
}