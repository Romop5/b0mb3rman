#pragma once

#include <chrono>
#include <queue>
#include <random>
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
                                         event::EntityCollide,
                                         event::FireTerminated>(*this);
  }

public:
  auto handle(const event::GameStarted& event) -> void
  {
    world_.clear();
    world_.create(Entity::Type::player)
      .set_tileset("characters/farmer.json")
      .set_tile(2)
      .set_origin({ 0, 0 })
      .set_max_speed(10.0f);

    world_.create(Entity::Type::pickup).set_tile(8).set_origin({ 5, 5 });
    world_.create(Entity::Type::pickup).set_tile(8).set_origin({ 15, 5 });
    world_.create(Entity::Type::pickup).set_tile(8).set_origin({ 8, 7 });
    world_.create(Entity::Type::pickup).set_tile(8).set_origin({ 3, 11 });

    hud_manager_.get_texts().clear();
    hud_manager_.get_texts().create_named(
      "status", HUDManager::Text{ "", "", 10, glm::vec2(0.5, 0.5), true });

    hud_manager_.get_texts()
      .get_or_create_default("status")
      .set_text("Okay, let's gos!")
      .set_fade_effect(HUDManager::Text::FadingEffect(2));
  }

  auto handle(const event::PlayerMoved& event) -> void
  {
    if (const auto player_id = world_.get_player_id()) {
      auto& player = world_.get_entity(player_id.value());

      auto update_visual = [&](unsigned int movement_category) {
        if (event.should_accelerate) {
          player.set_animation(movement_category);
        } else if (not player.controller_.any()) {
          player.set_tile(movement_category);
        }
      };
      switch (event.direction_) {
        case event::PlayerMoved::MoveDirection::left:
          spdlog::trace("Player moved {}, should_accelerate{}",
                        "left",
                        event.should_accelerate);
          player.controller_.moving_left = event.should_accelerate;
          update_visual(3);
          break;
        case event::PlayerMoved::MoveDirection::right:
          spdlog::trace("Player moved {}, should_accelerate{}",
                        "right",
                        event.should_accelerate);
          player.controller_.moving_right = event.should_accelerate;
          update_visual(1);
          break;
        case event::PlayerMoved::MoveDirection::up:
          spdlog::trace("Player moved {}, should_accelerate{}",
                        "up",
                        event.should_accelerate);
          player.controller_.moving_up = event.should_accelerate;
          update_visual(0);
          break;
        case event::PlayerMoved::MoveDirection::down:
          spdlog::trace("Player moved {}, should_accelerate{}",
                        "down",
                        event.should_accelerate);
          player.controller_.moving_down = event.should_accelerate;
          update_visual(2);
          break;
      }
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

    std::default_random_engine generator;
    std::normal_distribution<float> distribution(200, 50);

    for (signed int i = -1; i < 2; i++) {
      for (signed int j = -1; j < 2; j++) {
        const auto fire_id = world_.create(Entity::Type::fire)
                               .set_tileset("fire.json")
                               .set_animation(0)
                               .set_origin(bomb.aabb_.origin_ + glm::vec2(i, j))
                               .get_id();

        using namespace std::chrono_literals;
        event_distributor_.enqueue_event(
          event::FireTerminated{ fire_id },
          1000ms + std::chrono::milliseconds{
                     static_cast<int>(distribution(generator)) });
      }
    }

    /*const auto player_id = world_.get_player_id();
    if (player_id) {
      auto& player = world_.get_entity(player_id.value());
      if (player.aabb_.distance_l1(bomb.aabb_) < 5) {
        event_distributor_.enqueue_event(event::PlayerDied{});
      }
    }*/

    hud_manager_.get_texts()
      .get_or_create_default("status")
      .set_text("Bomb exploded!")
      .set_fade_effect(HUDManager::Text::FadingEffect{ 1 })
      .set_wave_effect(HUDManager::Text::WaveEffect{ 0, 5 });
  }

  auto handle(const event::FireTerminated& event) -> void
  {
    if (not world_.has_entity(event.actor_)) {
      return;
    }

    auto& fire = world_.get_entity(event.actor_);
    fire.flags_.set(Entity::marked_for_destruction);
  }

  auto handle(const event::BombPlanted& event) -> void
  {
    if (const auto player_id = world_.get_player_id()) {
      auto bomb_id = world_.create(Entity::Type::bomb).set_tile(12).get_id();

      auto& player = world_.get_entity(player_id.value());

      const auto coords = glm::round(player.aabb_.origin_);
      world_.get_entity(bomb_id).set_origin(coords);

      using namespace std::chrono_literals;
      event_distributor_.enqueue_event(event::BombExploded{ bomb_id }, 2000ms);

      spdlog::trace("Planting bomb at ({}, {})", coords.x, coords.y);

      hud_manager_.get_texts()
        .get_or_create_default("status")
        .set_text("Bomb planted!")
        .set_fade_effect(HUDManager::Text::FadingEffect{ 1 })
        .set_wave_effect(HUDManager::Text::WaveEffect{ 0, 5 });
    }
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
      } else if (entity_a.type_ == Entity::Type::fire ||
                 entity_b.type_ == Entity::Type::fire) {

        event_distributor_.enqueue_event(event::PlayerDied{});
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