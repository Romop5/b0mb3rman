#include <catch2/catch_test_macros.hpp>

#include <utils/entity_registry.hpp>

TEST_CASE("utils::EntityRegistry: : constructor: basic", "entity_registry")
{
  struct Entity
  {
    using Id = unsigned;

    unsigned data{ 1337 };
  };

  utils::EntityRegistry<Entity> registry;
  REQUIRE(registry.begin() == registry.end());
  REQUIRE(registry.get_entries().size() == 0);

  {
    auto entity = registry.create(Entity{});
    REQUIRE(entity.data == 1337);
  }
}
