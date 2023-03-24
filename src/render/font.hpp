#pragma once

#include <memory>
#include <string>

#include <glm/glm.hpp>

namespace render {

/**
 * @brief Text rendering in OpenGL via FreeType
 *
 */
class Font
{
public:
  // Fwd declaration
  class FontImpl;

  Font() = default;
  explicit Font(const std::string& font_name);
  ~Font();

  auto draw_text(const std::string& text, glm::vec2 start) -> void;

private:
  std::unique_ptr<FontImpl> pimpl_;
};
} // namespace