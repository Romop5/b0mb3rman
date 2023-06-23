#pragma once

#include <fmt/format.h>
#include <fmt/printf.h>
#include <iostream>

#define UTILS_TRACE_GRAPH_FN_IMPL_OLD(fn_name, constness, ...)                 \
  template<typename... Args>                                                   \
  auto fn_name(Args... args) constness                                         \
  {                                                                            \
    return do_and_dump(__func__,                                               \
                       std::bind(std::mem_fn(&G::fn_name),                     \
                                 graph_,                                       \
                                 std::forward<Args>(args)...),                 \
                       std::forward<Args>(args)...);                           \
  }

#define UTILS_TRACE_GRAPH_FN_IMPL(fn_name, constness, ...)                     \
  template<typename... Args>                                                   \
  auto fn_name(Args... args) constness                                         \
  {                                                                            \
    return do_and_dump(                                                        \
      __func__,                                                                \
      [&]() { return graph_.fn_name(std::forward<Args>(args)...); },           \
      std::forward<Args>(args)...);                                            \
  }

#define UTILS_TRACE_GRAPH_FN(...) UTILS_TRACE_GRAPH_FN_IMPL(__VA_ARGS__, )

namespace utils {
template<typename G>
class TracedGraph
{
public:
  using Graph = G;
  using NodeId = typename Graph::NodeId;
  using Edge = typename Graph::Edge;
  using EdgeStorage = typename Graph::EdgeStorage;

  TracedGraph() = default;
  TracedGraph(std::unordered_set<NodeId> vertices,
              std::unordered_set<Edge, detail::pair_hash> edges)
  {
    graph_ = Graph{ vertices, edges };
  }

  UTILS_TRACE_GRAPH_FN(has_edge, const);
  UTILS_TRACE_GRAPH_FN(get_edge_data, const);

  UTILS_TRACE_GRAPH_FN(empty_edges, const);
  UTILS_TRACE_GRAPH_FN(empty_vertices, const);
  UTILS_TRACE_GRAPH_FN(add_vertex);
  UTILS_TRACE_GRAPH_FN(has_vertex, const);
  UTILS_TRACE_GRAPH_FN(remove_vertex);
  UTILS_TRACE_GRAPH_FN(get_neighbours, const);

  auto add_edge(NodeId a, NodeId b, EdgeStorage data = {}) -> void
  {
    return do_and_dump(
      __func__, [&]() { return graph_.add_edge(a, b, data); }, a, b, data);
  }

  UTILS_TRACE_GRAPH_FN(remove_edge);
  // UTILS_TRACE_GRAPH_FN(get_vertices, const);
  // UTILS_TRACE_GRAPH_FN(get_edges, const);

  auto get_vertices() const { return graph_.get_vertices(); }
  auto get_edges() const { return graph_.get_edges(); }

protected:
  using Operand = std::variant<std::monostate, NodeId>;
  using OperationResult =
    std::variant<std::monostate, bool, std::unordered_set<NodeId>>;

  template<typename F, typename... Args>
  auto do_and_dump(const std::string& operation_name,
                   F functor,
                   Args... args) const
  {
    using FunctorType = decltype(functor());
    if constexpr (std::is_void<FunctorType>()) {
      dump_operation(operation_name, { args... });
      return std::invoke(functor);
    } else {
      const auto result = std::invoke(functor);
      dump_operation(operation_name, { args... }, result);
      return result;
    }
  }

  /**
   * @brief Generic method for dumping accessed graph's operation
   *
   * @param operation function name (e.g. add_edge)
   * @param first  operator
   * @param second  (optional) operand
   * @param result
   */
  auto dump_operation(const std::string& operation_name,
                      std::vector<Operand> operands = {},
                      OperationResult result = {}) const -> void
  {
    std::cout << fmt::format("op({})", operation_name);
    for (const auto& operand : operands) {
      if (std::holds_alternative<std::monostate>(operand)) {
        continue;
      }
      std::cout << fmt::format("{} ", std::get<NodeId>(operand));
    }
    if (not std::holds_alternative<std::monostate>(result)) {
      if (const auto val_ptr = std::get_if<bool>(&result)) {
        std::cout << "=> " << *val_ptr;
      } else if (const auto val_ptr =
                   std::get_if<std::unordered_set<NodeId>>(&result)) {
        std::cout << "=> [";
        for (const auto& node_id : *val_ptr) {
          std::cout << node_id << " ";
        }
        std::cout << "]";
      } else {
      }
    }
    std::cout << std::endl;
  }

private:
  Graph graph_;
};

} // namespace utils