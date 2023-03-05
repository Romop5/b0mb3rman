#pragma once

#include <cassert>
#include <unordered_map>
#include <utility>

#include <glbinding-aux/types_to_string.h>
#include <glbinding/gl/gl.h>
#include <utils/exceptions.hpp>

namespace render {
// Fwd
template<gl::GLenum>
class Resource;

// Strong typing
using Shader = render::Resource<gl::GL_SHADER>;
using Program = render::Resource<gl::GL_PROGRAM>;
using Buffer = render::Resource<gl::GL_BUFFER>;
using VertexArray = render::Resource<gl::GL_VERTEX_ARRAY>;

/**
 * @brief Generic OpenGL Resource with ownership
 *
 */
template<gl::GLenum _Type>
class Resource
{
public:
  Resource() = default;

  Resource(const Resource&) = delete;
  Resource& operator=(const Resource&) = delete;

  Resource(Resource&& other) { std::swap(resource_, other.resource_); }
  Resource& operator=(Resource&& other)
  {
    std::swap(resource_, other.resource_);
    return *this;
  }

  ~Resource()
  {
    if (resource_) {
      if constexpr (_Type == gl::GL_PROGRAM) {
        gl::glDeleteProgram(resource_);
      } else if constexpr (_Type == gl::GL_SHADER) {
        gl::glDeleteShader(resource_);
      } else if constexpr (_Type == gl::GL_BUFFER) {
        gl::glDeleteBuffers(1, &resource_);
      } else if constexpr (_Type == gl::GL_VERTEX_ARRAY) {
        gl::glDeleteVertexArrays(1, &resource_);
      } else {
        static_assert(_Type == gl::GL_PROGRAM,
                      "Unsupported type of render::Resource,"
                      " please, write here a valid destructor case");
      }
    }
  }

  [[nodiscard]] operator gl::GLuint() const
  {
    assert(resource_);
    return resource_;
  }

  friend Program create_program();
  friend Shader create_shader(gl::GLenum type);
  friend Buffer create_buffer();
  friend VertexArray create_vertex_array();

private:
  Resource(gl::GLuint id)
    : resource_{ id }
  {}
  gl::GLuint resource_{ 0 };
};

[[nodiscard]] Program
create_program()
{
  return Resource<gl::GL_PROGRAM>(gl::glCreateProgram());
}

[[nodiscard]] Shader
create_shader(gl::GLenum type)
{
  return Resource<gl::GL_SHADER>(gl::glCreateShader(type));
}

[[nodiscard]] Buffer
create_buffer()
{
  gl::GLuint id;
  gl::glGenBuffers(1, &id);

  return Resource<gl::GL_BUFFER>(id);
}

[[nodiscard]] VertexArray
create_vertex_array()
{
  gl::GLuint id;
  gl::glGenVertexArrays(1, &id);

  return Resource<gl::GL_VERTEX_ARRAY>(id);
}

[[nodiscard]] std::string
enum_to_str(gl::GLenum enum_constant)
{
  std::stringstream ss;
  ss << enum_constant;
  return ss.str();
}

} // namespace render