#include <render/tile_renderer.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include <glm/gtx/transform.hpp>

using namespace render;

render::TileRenderer::TileRenderer(Program&& program)
  : program_{ std::move(program) }
{
  gl::glUseProgram(program_);
}

auto
render::TileRenderer::get_screen_size() const -> glm::vec2
{
  gl::GLfloat viewport[4];
  gl::glGetFloatv(gl::GL_VIEWPORT, viewport);

  const auto& screen_width = viewport[2];
  const auto& screen_height = viewport[3];
  return glm::vec2(screen_width, screen_height);
}

auto
render::TileRenderer::set_projection_matrix(float min_x,
                                            float min_y,
                                            float max_x,
                                            float max_y) -> void
{
  gl::glUseProgram(program_);
  auto matrix = glm::ortho(min_x, max_x, min_y, max_y);
  {
    auto location = gl::glGetUniformLocation(program_, "projection");
    gl::glUniformMatrix4fv(location, 1, gl::GL_FALSE, glm::value_ptr(matrix));
  }
}

auto
render::TileRenderer::bind_tileset(const Tileset& tileset) -> void
{
  gl::glActiveTexture(gl::GL_TEXTURE0);
  gl::glBindTexture(gl::GL_TEXTURE_2D, tileset.texture_);
  gl::glUseProgram(program_);

  {
    auto location = gl::glGetUniformLocation(program_, "tile_texture");
    gl::glUniform1ui(location, 0);
  }

  {
    assert(tileset.tile_size_x_);
    auto location = gl::glGetUniformLocation(program_, "tile_count_x");
    gl::glUniform1ui(location, tileset.tile_size_x_);
  }

  {
    assert(tileset.tile_size_y_);
    auto location = gl::glGetUniformLocation(program_, "tile_count_y");
    gl::glUniform1ui(location, tileset.tile_size_y_);
  }
}

auto
render::TileRenderer::draw_quad(float x1,
                                float y1,
                                float x2,
                                float y2,
                                unsigned tile_index) -> void
{
  {
    auto location = gl::glGetUniformLocation(program_, "tile_id");
    gl::glUniform1ui(location, tile_index);
  }

  {
    // auto matrix = glm::scale(glm::vec3(x2 - x1, y2 - y1, 1.0)) *
    // glm::translate(glm::vec3((x1 + x2) / -2.0, (y1 + y2) / -2.0, 0));
    auto quad = glm::vec4{ x1, y1, x2, y2 };
    auto location = gl::glGetUniformLocation(program_, "quad");
    gl::glUniform4fv(location, 1, glm::value_ptr(quad));
  }

  gl::glUseProgram(program_);
  quad_.draw();
}
auto
render::TileRenderer::draw_quad(const glm::vec2& position,
                                const glm::vec2& size,
                                unsigned tile_index) -> void
{
  const auto bottom_right = position + size;
  draw_quad(
    position[0], position[1], bottom_right[0], bottom_right[1], tile_index);
}

render::TileRenderer::Quad::Quad()
  : vao_{ create_vertex_array() }
  , vbo_{ create_buffer() }
  , veo_{ create_buffer() }
{

  gl::glBindVertexArray(vao_);

  gl::glBindBuffer(gl::GL_ARRAY_BUFFER, vbo_);
  gl::glBufferData(gl::GL_ARRAY_BUFFER,
                   vertices_.size() * sizeof(TileVertex),
                   vertices_.data(),
                   gl::GL_STATIC_DRAW);

  gl::glBindBuffer(gl::GL_ELEMENT_ARRAY_BUFFER, veo_);
  gl::glBufferData(gl::GL_ELEMENT_ARRAY_BUFFER,
                   indices_.size() * sizeof(TileVertex),
                   indices_.data(),
                   gl::GL_STATIC_DRAW);

  gl::glVertexAttribPointer(
    0, 3, gl::GL_FLOAT, false, sizeof(float) * 3, static_cast<void*>(0));
  gl::glEnableVertexAttribArray(0);

  gl::glBindVertexArray(0);
  gl::glBindBuffer(gl::GL_ARRAY_BUFFER, 0);
  gl::glBindBuffer(gl::GL_ELEMENT_ARRAY_BUFFER, 0);
}

auto
render::TileRenderer::Quad::draw() -> void
{
  gl::glBindVertexArray(vao_);
  gl::glDrawArrays(gl::GL_TRIANGLE_STRIP, 0, 4);
  gl::glBindVertexArray(0);
}