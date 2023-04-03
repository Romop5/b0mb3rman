#include <bm/hud_manager.hpp>

using namespace bm;

HUDManager::HUDManager(render::FontRenderer& font_render)
  : font_render_{ font_render }
{
}

auto
HUDManager::render(std::chrono::milliseconds delta) -> void
{
  for (auto& [id, entity] : texts_) {
    if (entity.fading_effect_) {
      entity.fading_effect_->update(delta);
    }

    if (entity.wave_effect_) {
      entity.wave_effect_->update(delta);
    }
  }

  for (const auto [id, entity] : texts_) {
    if (entity.fading_effect_ && entity.fading_effect_->is_text_visible()) {

      const auto position =
        entity.wave_effect_
          ? entity.wave_effect_->apply_effect(entity.position_)
          : entity.position_;
      font_render_.draw_text(
        entity.text_, position, entity.is_position_relative_);
    }
  }
}

auto
HUDManager::get_texts() -> utils::EntityNamedRegistry<Text>&
{
  return texts_;
}