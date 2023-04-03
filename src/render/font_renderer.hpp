#pragma once

#include <memory>
#include <string>

#include <render/viewport.hpp>

#include <glm/glm.hpp>

namespace render {

/**
 * @brief Text rendering in OpenGL via FreeType
 *
 */
class FontRenderer
{
public:
  // Fwd declaration
  class FontRendererImpl;

  FontRenderer() = default;
  explicit FontRenderer(const Viewport& viewport,
                        const std::string& default_font_name);
  ~FontRenderer();

  auto compute_text_metrics(const std::string& text) -> glm::vec2;
  auto draw_text(const std::string& text,
                 glm::vec2 origin,
                 bool position_relative = false) -> void;

private:
  std::unique_ptr<FontRendererImpl> pimpl_;
};
} // namespace