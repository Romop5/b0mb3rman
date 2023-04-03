#pragma once

#include <chrono>
#include <optional>

#include <render/font_renderer.hpp>
#include <utils/entity_registry.hpp>

namespace bm {

class HUDManager
{
public:
  struct Text
  {
    struct FadingEffect
    {
      /// @brief Current fade state (1: invisible, 0: visible)
      float fade_percentage{ 0 };
      /// Units (of fade) per second
      float fade_delta{ 0 };

      explicit FadingEffect(float seconds_to_fade_out)
        : fade_percentage{}
        , fade_delta{ 1.0f / seconds_to_fade_out }
      {
      }

      auto update(std::chrono::milliseconds delta) -> void
      {
        const auto rate_per_millisecond = fade_delta / 1000.0f;
        fade_percentage += delta.count() * rate_per_millisecond;
      }
      auto is_text_visible() const -> bool { return fade_percentage < 1.0; }
    };

    struct WaveEffect
    {
      /// @brief  Phase (in radians)
      float phase;
      /// Radians (of phase) per second
      float effect_delta;

      auto update(std::chrono::milliseconds delta) -> void
      {
        const auto rate_per_millisecond = effect_delta / 1000.0f;
        phase += delta.count() * rate_per_millisecond;
      }

      auto apply_effect(glm::vec2 position) const -> glm::vec2
      {
        return glm::vec2(position.x, position.y *= 1.0 + 0.1 * std::sin(phase));
      }
    };

    // std::string name as a key
    using Id = std::string;

    std::string text_;
    std::string font_;
    float font_size_;

    /* Position */
    glm::vec2 position_;
    bool is_position_relative_{ false };

    /* Effects */
    std::optional<FadingEffect> fading_effect_;
    std::optional<WaveEffect> wave_effect_;

    auto set_text(std::string text) -> Text&
    {
      text_ = text;
      return *this;
    }

    auto set_fade_effect(FadingEffect effect) -> Text&
    {
      fading_effect_.emplace(effect);
      return *this;
    }

    auto set_wave_effect(WaveEffect effect) -> Text&
    {
      wave_effect_.emplace(effect);
      return *this;
    }
  };

  explicit HUDManager(render::FontRenderer& font_render);

  auto render(std::chrono::milliseconds delta) -> void;
  auto get_texts() -> utils::EntityNamedRegistry<Text>&;

private:
  render::FontRenderer& font_render_;
  utils::EntityNamedRegistry<Text> texts_;
};

} // namespace bm