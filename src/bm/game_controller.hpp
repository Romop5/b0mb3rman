#pragma once

#include <chrono>
#include <queue>
#include <variant>
#include <vector>

#include <bm/entity.hpp>
#include <bm/event_distributor.hpp>
#include <bm/events.hpp>
#include <bm/hud_manager.hpp>
#include <bm/world.hpp>

namespace bm {

/**
 * @brief Event-based game logic processing
 */
class GameController
{

public:
  GameController(EventDistributor& event_distributor,
                 HUDManager& hud_manager,
                 World& world)
    : event_distributor_{ event_distributor }
    , hud_manager_{ hud_manager }
    , world_{ world }
  {
    event_distributor_.registry_listener<event::GameStarted,
                                         event::PlayerMoved,
                                         event::PlayerDied,
                                         event::BombExploded,
                                         event::BombPlanted,
                                         event::EntityCollide>(*this);
  }

public:
  auto handle(const event::GameStarted& event) -> void
  {
    world_.clear();
    world_.create(Entity::Type::player).set_tile(10).set_origin({ 0, 0 });
    world_.create(Entity::Type::pickup).set_tile(8).set_origin({ 5, 5 });
    world_.create(Entity::Type::pickup).set_tile(8).set_origin({ 15, 5 });
    world_.create(Entity::Type::pickup).set_tile(8).set_origin({ 8, 7 });
    world_.create(Entity::Type::pickup).set_tile(8).set_origin({ 3, 11 });

    hud_manager_.get_texts().clear();
    hud_manager_.get_texts().create_named(
      "status", HUDManager::Text{ "", "", 10, glm::vec2(0.5, 0.9), true });

    hud_manager_.get_texts()
      .get_or_create_default("status")
      .set_text("Okay, let's gos!")
      .set_fade_effect(HUDManager::Text::FadingEffect(2));
  }

  auto handle(const event::PlayerMoved& event) -> void
  {
    if (const auto player_id = world_.get_player_id()) {
      auto& player = world_.get_entity(player_id.value());
      auto& entity_origin = player.aabb_.origin_;
      entity_origin += event.direction_;
      // TODO: clamp entity_origin to world's dimensions
    } else {
      spdlog::debug("Missing player entity ingame");
    }
  }

  auto handle(const event::PlayerDied& event) -> void
  {
    if (const auto player_id = world_.get_player_id()) {
      auto& player = world_.get_entity(player_id.value());
      player.flags_.set(Entity::Flags::marked_for_destruction);
    }

    hud_manager_.get_texts()
      .get_or_create_default("status")
      .set_text("Game over, bastard!")
      .set_fade_effect(HUDManager::Text::FadingEffect{ 2 })
      .set_wave_effect(HUDManager::Text::WaveEffect{ 0, 3 });

    using namespace std::chrono_literals;
    event_distributor_.enqueue_event(event::GameStarted{}, 2000ms);
  }

  auto handle(const event::BombExploded& event) -> void
  {

    if (not world_.has_entity(event.actor_)) {
      return;
    }

    auto& bomb = world_.get_entity(event.actor_);
    bomb.flags_.set(Entity::Flags::marked_for_destruction);

    const auto player_id = world_.get_player_id();
    if (player_id) {
      auto& player = world_.get_entity(player_id.value());
      if (player.aabb_.distance_l1(bomb.aabb_) < 5) {
        event_distributor_.enqueue_event(event::PlayerDied{});
      }
    }

    hud_manager_.get_texts()
      .get_or_create_default("status")
      .set_text("Bomb exploded!")
      .set_fade_effect(HUDManager::Text::FadingEffect{ 2 })
      .set_wave_effect(HUDManager::Text::WaveEffect{ 0, 3 });
  }

  auto handle(const event::BombPlanted& event) -> void
  {
    const auto coords = glm::vec2{ 0, 0 };
    auto bomb_id = world_.create(Entity::Type::bomb)
                     .set_origin(coords)
                     .set_tile(12)
                     .get_id();

    if (const auto player_id = world_.get_player_id()) {
      auto& player = world_.get_entity(player_id.value());
      world_.get_entity(bomb_id).set_origin(player.aabb_.origin_);
    }

    using namespace std::chrono_literals;
    event_distributor_.enqueue_event(event::BombExploded{ bomb_id }, 2000ms);

    spdlog::trace("Planting bomb at ({}, {})", coords.x, coords.y);

    hud_manager_.get_texts()
      .get_or_create_default("status")
      .set_text("Bomb planted!")
      .set_fade_effect(HUDManager::Text::FadingEffect{ 2 })
      .set_wave_effect(HUDManager::Text::WaveEffect{ 0, 3 });
  }

  auto handle(const event::EntityCollide& event) -> void
  {
    if (entity_exist(event.actor_a_, event.actor_b_)) {
      auto& entity_a = world_.get_entity(event.actor_a_);
      auto& entity_b = world_.get_entity(event.actor_b_);

      if (entity_a.type_ == Entity::Type::pickup ||
          entity_b.type_ == Entity::Type::pickup) {
        auto& pickup =
          (entity_a.type_ == Entity::Type::pickup) ? entity_a : entity_b;
        pickup.flags_.set(Entity::Flags::marked_for_destruction);

        spdlog::debug("Picked up a pickup");
      }
    }
  }

protected:
  template<typename... Args>
  auto entity_exist(Entity::Id id, Args... args) -> bool
  {
    if constexpr (sizeof...(args) == 0) {
      return world_.has_entity(id);
    } else {
      return entity_exist(args...) && world_.has_entity(id);
    }
  }

private:
  EventDistributor& event_distributor_;

  HUDManager& hud_manager_;
  World& world_;
};
} // namespace bm