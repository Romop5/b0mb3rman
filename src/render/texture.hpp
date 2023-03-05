#pragma once

#include <cassert>
#include <filesystem>
#include <memory>
#include <vector>

#include <FreeImage.h>
#include <glbinding/gl/gl.h>
#include <spdlog/spdlog.h>

namespace render {
class Texture
{
public:
  Texture() = default;

  unsigned int width_{ 0 };
  unsigned int height_{ 0 };
  gl::GLuint id_{ 0 };

  operator gl::GLuint() const
  {
    assert(id_);
    return id_;
  }
};

auto
load_texture_from_file(const std::filesystem::path& path) -> Texture;
} // namespace render