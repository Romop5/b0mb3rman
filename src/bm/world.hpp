#pragma once

#include <chrono>
#include <vector>

#include <bm/entity.hpp>
#include <bm/event_distributor.hpp>

namespace bm {
class World
{
public:
  explicit World(EventDistributor& event_distributor);

  template<typename... Args>
  auto create(Args... args) -> Entity&
  {
    auto id = next_id_++;
    entities_.emplace(id, std::move(Entity{ id, args... }));
    return get_entity(id);
  }

  auto begin() { return entities_.begin(); }
  auto end() { return entities_.end(); }

  auto get_player_id() -> std::optional<Entity::Id>;
  auto get_entity(Entity::Id id) -> Entity&;
  auto has_entity(Entity::Id id) const -> bool;
  auto clear() -> void;

  auto delete_marked_entities() -> void;
  auto update(std::chrono::milliseconds delta) -> void;

protected:
  auto detect_collisions() -> void;

private:
  EventDistributor& event_distributor_;

  Entity::Id next_id_{ 0 };
  std::unordered_map<Entity::Id, Entity> entities_;
};

} // namespace bm
