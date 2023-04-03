#pragma once

#include <unordered_map>

namespace utils {

enum class EntityRegistryKeyPolicy
{
  generated, // generated IDs
  named      // pre-specified IDs (names)
};

/**
 * @brief Type-generic entity registry with unique key
 *
 * @tparam Entity
 */
template<typename Entity,
         typename Key = typename Entity::Id,
         EntityRegistryKeyPolicy Mechanism = EntityRegistryKeyPolicy::generated>
class EntityRegistry
{

public:
  template<typename... Args>
  auto create(Args... args) -> Entity&;

  template<typename... Args>
  auto create_named(Key name, Args... args) -> Entity&;

  auto get_or_create_default(Key name) -> Entity&;

  auto get_entity(Key id) -> Entity&;
  auto has_entity(Key id) const -> bool;
  auto clear() -> void;
  auto get_entries() -> const std::unordered_map<Key, Entity>&;
  auto begin() { return entities_.begin(); }
  auto end() { return entities_.end(); }

protected:
  auto generate_next_key() -> Key;

private:
  Key next_id_{ 0 };
  std::unordered_map<Key, Entity> entities_;
};

template<typename Entity>
using EntityNamedRegistry =
  EntityRegistry<Entity, std::string, EntityRegistryKeyPolicy::named>;

//=============================================================================

template<typename Entity, typename Key, EntityRegistryKeyPolicy Mechanism>
template<typename... Args>
auto
EntityRegistry<Entity, Key, Mechanism>::create(Args... args) -> Entity&
{
  auto id = generate_next_key();
  entities_.emplace(id, std::move(Entity{ args... }));
  return get_entity(id);
}

template<typename Entity, typename Key, EntityRegistryKeyPolicy Mechanism>
template<typename... Args>
auto
EntityRegistry<Entity, Key, Mechanism>::create_named(Key name, Args... args)
  -> Entity&
{
  entities_.emplace(name, std::move(Entity{ args... }));
  return get_entity(name);
}

template<typename Entity, typename Key, EntityRegistryKeyPolicy Mechanism>
auto
EntityRegistry<Entity, Key, Mechanism>::get_or_create_default(Key name)
  -> Entity&
{
  if (has_entity(name)) {
    return get_entity(name);
  } else {
    return create_named(name, Entity{});
  }
}

template<typename Entity, typename Key, EntityRegistryKeyPolicy Mechanism>
auto
EntityRegistry<Entity, Key, Mechanism>::get_entity(Key id) -> Entity&
{
  return entities_.at(id);
}

template<typename Entity, typename Key, EntityRegistryKeyPolicy Mechanism>
auto
EntityRegistry<Entity, Key, Mechanism>::has_entity(Key id) const -> bool
{
  return entities_.count(id) > 0;
}

template<typename Entity, typename Key, EntityRegistryKeyPolicy Mechanism>
auto
EntityRegistry<Entity, Key, Mechanism>::clear() -> void
{
  return entities_.clear();
}

template<typename Entity, typename Key, EntityRegistryKeyPolicy Mechanism>
auto
EntityRegistry<Entity, Key, Mechanism>::get_entries()
  -> const std::unordered_map<Key, Entity>&
{
  return entities_;
}

template<typename Entity, typename Key, EntityRegistryKeyPolicy Mechanism>
auto
EntityRegistry<Entity, Key, Mechanism>::generate_next_key() -> Key
{
  static_assert(
    Mechanism == EntityRegistryKeyPolicy::generated,
    "create() without name is allowed only for generated (managed) entities");

  return next_id_++;
}
} // namespace utils