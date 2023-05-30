#pragma once

#include <functional>
#include <memory>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>

#include <utils/graph.hpp>
#include <utils/graph_algorithms.hpp>

namespace utils {

namespace detail {
template<typename Type, typename... Types>
class ArgumentPackDescriptorImpl
{
public:
  static auto as_type_index() -> std::vector<std::type_index>
  {
    if constexpr (sizeof...(Types) == 0) {
      return { typeid(Type) };
    } else {
      auto result = ArgumentPackDescriptorImpl<Types...>::as_type_index();
      result.push_back(ArgumentPackDescriptorImpl<Type>::as_type_index()[0]);
      return result;
    }
  }
};

/**
 * @brief Helper: translates arg. pack to a vector of std::type_index
 *
 * @tparam Types
 */
template<typename... Types>
class ArgumentPackDescriptor
{
public:
  static auto as_type_index() -> std::vector<std::type_index>
  {
    if constexpr (sizeof...(Types) == 0) {
      return {};
    } else {
      return ArgumentPackDescriptorImpl<Types...>::as_type_index();
    }
  }
};
/**
 * @brief Helper: assigns a stable ID to set's value
 *
 * @tparam T
 */
template<typename T>
class EnumerableSet
{
public:
  using Index = unsigned long;
  auto count(T key) const { return map_.count(key); }
  auto has(Index index) const -> bool { return index < current_index; }
  auto insert(T key) -> Index
  {
    if (map_.count(key)) {
      return map_.at(key);
    }
    map_[key] = current_index++;
    return map_[key];
  }

  auto at(T key) const -> Index { return map_.at(key); }
  auto at(Index index) const -> T
  {
    if (not has(index)) {
      throw std::runtime_error("Invalid index");
    }

    const auto index_iterator =
      std::find_if(map_.begin(), map_.end(), [index](auto id) {
        return id.second == index;
      });
    if (index_iterator == map_.end()) {
      throw std::logic_error("Expected index in map");
    }
    return index_iterator->first;
  }

  auto begin() { return map_.begin(); }
  auto end() { return map_.end(); }

private:
  Index current_index = 0;
  std::unordered_map<T, Index> map_;
};

} // namespace detail

class ServiceLocator
{
public:
  /// @brief Initializes all services given registered depedencis
  /// @throw if it is impossible to do so (e.g. due to circular deps)
  auto initialize() -> void
  {
    // Construct a unique mapping of registered types, from type_index to an
    // Index (unsigned long)
    const auto dependency_to_node_id = enumerate_dependencies();

    // Given unique indices and registred depdendencies, construct an oriented
    // graph of dependencies
    utils::OrientedGraph<> dependency_graph =
      construct_dependency_graph(dependency_to_node_id);

    // Pseudo-linearize the graph's nodes
    auto dependency_order =
      utils::graph_algorithms::linearly_order_nodes(dependency_graph);
    std::reverse(dependency_order.begin(), dependency_order.end());

    // Initializer services by linearly processing the ordered set

    for (const auto& service_id : dependency_order) {
      const auto service_type_index = dependency_to_node_id.at(service_id);
      auto factory_method = registered_factories_.at(service_type_index);
      services_[service_type_index] = std::invoke(factory_method);
    }
  }

  template<typename Interface, typename Instance, typename... Args>
  auto register_interface(Args&&... args) -> void
  {
    registered_dependecies_[typeid(Interface)] = Instance::get_depedencies();
    registered_factories_[typeid(Interface)] = [&]() {
      return std::make_shared<Instance>(std::forward<Args>(args)...);
    };
  }

  template<typename Interface>
  auto has_interface() -> bool
  {
    return services_.count(typeid(Interface));
  }

  template<typename Interface>
  auto get() -> Interface*
  {
    return static_cast<Interface*>(services_.at(typeid(Interface)).get());
  }

private:
  auto enumerate_dependencies() const -> detail::EnumerableSet<std::type_index>
  {
    detail::EnumerableSet<std::type_index> dependency_to_node_id;
    for (const auto& [service, service_dependencies] :
         registered_dependecies_) {
      dependency_to_node_id.insert(service);

      for (const auto& depedency : service_dependencies) {
        dependency_to_node_id.insert(depedency);
      }
    }
    return dependency_to_node_id;
  }

  auto construct_dependency_graph(
    detail::EnumerableSet<std::type_index> indexed_depedencies) const
    -> utils::OrientedGraph<>
  {
    utils::OrientedGraph<> dependency_graph;

    for (const auto& [service, service_dependencies] :
         registered_dependecies_) {
      for (const auto& depedency : service_dependencies) {
        const auto service_id = indexed_depedencies.at(service);
        const auto dep_id = indexed_depedencies.at(depedency);
        dependency_graph.add_edge(service_id, dep_id);
      }
    }
    return dependency_graph;
  }

private:
  using ServiceFactoryMethod = std::function<std::shared_ptr<void>()>;
  using ServiceDependecies = std::vector<std::type_index>;

  /// @brief Initializion-only: list of factory methods for instancing Interface
  std::unordered_map<std::type_index, ServiceFactoryMethod>
    registered_factories_;
  /// @brief Initializion-only: list of depdendencies for instancing Interface
  std::unordered_map<std::type_index, ServiceDependecies>
    registered_dependecies_;

  /// @brief Storage of initialized service
  std::unordered_map<std::type_index, std::shared_ptr<void>> services_;
};

template<typename... Depedencies>
class Service
{
public:
  using DependencyList = utils::detail::ArgumentPackDescriptor<Depedencies...>;

  static auto get_depedencies() -> std::vector<std::type_index>
  {
    return utils::detail::ArgumentPackDescriptor<
      Depedencies...>::as_type_index();
  }
};

} // namespace utils