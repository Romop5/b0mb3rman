#include <render/tile_program.hpp>

using namespace render;

namespace {

/**
 * @brief OpenGL-like char buffer with ownership
 *
 * @param length (without summing up NULL byte)
 * @return std::unique_ptr<gl::GLchar[]>
 */
auto
allocate_opengl_string(unsigned length) -> std::vector<gl::GLchar>
{
  return std::vector<gl::GLchar>(length + 1);
}

auto
convert_string_to_gl_string(const std::string& str)
{
  const auto result_length = str.length() + 1;
  std::vector<gl::GLchar> result(result_length);
  std::memcpy(result.data(), str.data(), result_length);
  return result;
}

auto
convert_gl_string_to_printable_string(gl::GLchar const* str) -> std::string
{
  const auto len = std::strlen(str);
  std::string result(len + 1, ' ');
  for (unsigned i = 0; i < len; i++) {
    result[i] = str[i];
  }
  return result;
}

auto
throw_on_error(const std::string& msg) -> void
{
  if (gl::glGetError() != gl::GL_NO_ERROR)
    throw std::runtime_error(msg);
}

template<typename T>
auto
exception_assert(T expr, const std::string& msg) -> void
{
  if (!expr) {
    throw std::runtime_error(msg);
  }
}

} // namespace

auto
render::load_shader(gl::GLenum type, const std::string& code) -> Shader
{
  auto shader = render::create_shader(type);
  auto converted_code = convert_string_to_gl_string(code);
  const std::array<const gl::GLchar*, 1> codes = { converted_code.data() };
  const std::array<const gl::GLint, 1> lengths = { converted_code.size() };

  gl::glShaderSource(shader, 1, codes.data(), lengths.data());
  throw_on_error("Failed to load shader's resource");

  gl::glCompileShader(shader);
  throw_on_error("Failed to compile shader");

  gl::GLboolean is_compiled_flag;
  gl::glGetShaderiv(shader, gl::GL_COMPILE_STATUS, &is_compiled_flag);
  throw_on_error("Failed to get compile status");

  gl::GLsizei log_length;
  gl::glGetShaderiv(shader, gl::GL_INFO_LOG_LENGTH, &log_length);

  auto log = allocate_opengl_string(log_length);
  gl::glGetShaderInfoLog(shader, log_length, &log_length, log.data());
  throw_on_error("Failed to get shader log");

  if (!is_compiled_flag) {
    throw std::runtime_error(
      fmt::format("Compilation fail ({}): \n{}",
                  enum_to_str(type),
                  convert_gl_string_to_printable_string(log.data())));
  }

  return shader;
}

auto
render::link_program(const std::vector<Shader>& shaders) -> Program
{
  auto program = create_program();
  for (auto& shader : shaders) {
    gl::glAttachShader(program, shader);
    throw_on_error("Failed to attach shader");
  }

  gl::glLinkProgram(program);

  for (auto& shader : shaders) {
    gl::glDetachShader(program, shader);
    throw_on_error("Failed to deattach shader");
  }

  gl::GLboolean is_linked_flag;
  gl::glGetProgramiv(program, gl::GL_LINK_STATUS, &is_linked_flag);
  throw_on_error("Failed to get link status");

  gl::GLsizei log_length;
  gl::glGetProgramiv(program, gl::GL_INFO_LOG_LENGTH, &log_length);

  auto log = allocate_opengl_string(log_length);
  gl::glGetProgramInfoLog(program, log_length, &log_length, log.data());
  throw_on_error("Failed to get program log");

  if (!is_linked_flag) {
    throw std::runtime_error(
      fmt::format("Compilation fail: {}",
                  convert_gl_string_to_printable_string(log.data())));
  }

  return program;
}
