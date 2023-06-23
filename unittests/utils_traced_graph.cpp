#include <catch2/catch_test_macros.hpp>

#include <utils/graph.hpp>
#include <utils/graph_algorithms.hpp>
#include <utils/traced_graph.hpp>

TEST_CASE("utils::TracedGraph: basics", "traced_graph")
{
  utils::TracedGraph<utils::OrientedGraph<>> traced_graph;
  traced_graph.empty_edges();
  traced_graph.empty_vertices();

  traced_graph.add_vertex(0);
  traced_graph.add_edge(0, 1);
  traced_graph.add_edge(1, 2);
  traced_graph.add_edge(1, 8);
  traced_graph.add_edge(8, 0);

  traced_graph.get_neighbours(0);

  traced_graph.empty_edges();
  traced_graph.empty_vertices();

  auto reflexive_g = utils::graph_algorithms::make_transitive(
    utils::graph_algorithms::make_reflexive(traced_graph));
  reflexive_g.has_vertex(0);
}
