#include <render/resource.hpp>

#include <sstream>
#include <string>

using namespace render;

[[nodiscard]] Program
render::create_program()
{
  return Resource<gl::GL_PROGRAM>(gl::glCreateProgram());
}

[[nodiscard]] Shader
render::create_shader(gl::GLenum type)
{
  return Resource<gl::GL_SHADER>(gl::glCreateShader(type));
}

[[nodiscard]] Buffer
render::create_buffer()
{
  gl::GLuint id;
  gl::glGenBuffers(1, &id);

  return Resource<gl::GL_BUFFER>(id);
}

[[nodiscard]] VertexArray
render::create_vertex_array()
{
  gl::GLuint id;
  gl::glGenVertexArrays(1, &id);

  return Resource<gl::GL_VERTEX_ARRAY>(id);
}

[[nodiscard]] Texture
render::create_texture()
{
  gl::GLuint id;
  gl::glGenTextures(1, &id);

  return Resource<gl::GL_TEXTURE>(id);
}

[[nodiscard]] std::string
render::enum_to_str(gl::GLenum enum_constant)
{
  std::stringstream ss;
  ss << enum_constant;
  return ss.str();
}
