#pragma once

#include <exception>
#include <string>

#include <glbinding/glbinding.h>

namespace utils {

inline auto
throw_on_opengl_error(const std::string& msg) -> void
{
  if (gl::glGetError() != gl::GL_NO_ERROR)
    throw std::runtime_error(msg);
}

inline auto
clear_opengl_error() -> void
{
  gl::glGetError();
}

} // namespace utils