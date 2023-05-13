#pragma once

#include <bm/world.hpp>
#include <utils/aabb.hpp>
#include <utils/graph.hpp>

namespace bm {

class NavigationMesh
{
public:
  NavigationMesh(bm::interfaces::ICollisionWorld& world);

  auto update() -> void;

  auto is_reachable(glm::vec2 start, glm::vec2 end) -> bool;
  auto compute_path(glm::vec2 start, glm::vec2 end) -> std::vector<glm::vec2>;

private:
  auto compute_node_id(glm::vec2 position) -> utils::UnorientedGraph<>::NodeId;
  auto compute_position_from_node_id(utils::UnorientedGraph<>::NodeId id)
    -> glm::vec2;

  /**
   * @brief Caches computed graphs between update() calls
   */
  struct Cache
  {
    /// @brief Defines size of the world (needed for computing node IDs)
    utils::AABB boundary;
    /// @brief Defines topology of unobstructed tiles in game
    utils::UnorientedGraph<> graph;
    /// @brief Serves for speeding up rejection of path existance
    utils::UnorientedGraph<> strong_components;
  } cache_;

  bm::interfaces::ICollisionWorld& world_;
};
} // namespace bm