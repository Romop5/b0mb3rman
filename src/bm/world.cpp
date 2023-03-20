#include <bm/events.hpp>
#include <bm/world.hpp>

using namespace bm;

namespace {

} // namespace

World::World(EventDistributor& event_distributor)
  : event_distributor_{ event_distributor }
{
}

auto
World::get_player_id() -> std::optional<Entity::Id>
{
  for (auto& [id, entity] : entities_) {
    if (entity.type_ == Entity::Type::player) {
      return id;
    }
  }
  return {};
}

auto
World::get_entity(Entity::Id id) -> Entity&
{
  return entities_.at(id);
}

auto
World::has_entity(Entity::Id id) const -> bool
{
  return entities_.count(id);
}

auto
World::clear() -> void
{
  entities_.clear();
}

auto
World::delete_marked_entities() -> void
{
  const auto predicate = [](const Entity& entity) {
    return entity.flags_.test(Entity::Flags::marked_for_destruction);
  };

  // TODO: C++20 provides erase_if
  for (auto i = entities_.begin(), last = entities_.end(); i != last;) {
    if (predicate(i->second)) {
      i = entities_.erase(i);
    } else {
      ++i;
    }
  }
}

auto
World::detect_collisions() -> void
{
  // naive O(N^2) comparison
  for (const auto& [id_a, entity_a] : entities_) {
    for (const auto& [id_b, entity_b] : entities_) {
      if (id_b <= id_a)
        continue;

      // TODO: introduce collision groups & masks
      if (entity_a.type_ == entity_b.type_) {
        continue;
      }

      if (entity_a.flags_.test(Entity::Flags::marked_for_destruction) ||
          entity_b.flags_.test(Entity::Flags::marked_for_destruction)) {
        continue;
      }

      const auto& a = entity_a.aabb_;
      const auto& b = entity_b.aabb_;

      if (a.collide(b)) {
        spdlog::debug("Entity {} and {} collide", id_a, id_b);
        event_distributor_.enqueue_event(
          bm::event::EntityCollide{ id_a, id_b });
      }
    }
  }
}
