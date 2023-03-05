#pragma once

#include <memory>
#include <vector>

#include <FreeImage.h>
#include <glbinding/gl/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include <glm/gtx/transform.hpp>

#include <render/tile_program.hpp>
#include <render/tiled_map.hpp>
#include <render/tileset.hpp>

namespace render {
/**
 * @brief Renders a tile world
 *
 */
class TileRenderer
{
public:
  TileRenderer(Program&& program)
    : program_{ std::move(program) }
  {
    gl::glUseProgram(program_);
  }

  auto add_map([[maybe_unused]] const std::string& map_name,
               std::shared_ptr<TiledMap> map)
  {
    // TODO: use dictionary for maps
    map_ = std::move(map);
  }

  auto render() -> void
  {
    assert(map_);

    const auto& texture = map_->tileset_->texture_;
    // program_.bind_tileset(map_->tileset_);

    const auto screen_size = get_screen_size();

    const auto tile_width = screen_size[0] / map_->count_x;
    const auto tile_height = screen_size[1] / map_->count_y;

    set_projection_matrix(0, 0, screen_size[0], screen_size[1]);

    for (size_t y = 0; y < map_->count_y; y++) {
      for (size_t x = 0; x < map_->count_x; x++) {
        // Map (x,y) to <0, width*height)
        const auto tile_position_index = y * map_->count_x + x;
        const auto tile_texture_index =
          map_->tile_indices_.at(tile_position_index);

        const auto start_x = tile_width * x;
        const auto start_y = tile_height * y;

        draw_quad(start_x,
                  start_y,
                  start_x + tile_width,
                  start_y + tile_height,
                  tile_texture_index);
      }
    }
  }

private:
  auto get_screen_size() const -> glm::vec2
  {
    gl::GLfloat viewport[4];
    gl::glGetFloatv(gl::GL_VIEWPORT, viewport);

    const auto& screen_width = viewport[2];
    const auto& screen_height = viewport[3];
    return glm::vec2(screen_width, screen_height);
  }
  auto set_projection_matrix(float min_x, float min_y, float max_x, float max_y)
    -> void
  {
    auto matrix = glm::ortho(min_x, max_x, min_y, max_y);
    {
      auto location = gl::glGetUniformLocation(program_, "projection");
      gl::glUniformMatrix4fv(location, 1, gl::GL_FALSE, glm::value_ptr(matrix));
    }
  }

  auto draw_quad(float x1, float y1, float x2, float y2, unsigned tile_index)
    -> void
  {
    {
      gl::glActiveTexture(gl::GL_TEXTURE0);
      gl::glBindTexture(gl::GL_TEXTURE_2D, map_->tileset_->texture_);

      auto location = gl::glGetUniformLocation(program_, "tile_texture");
      gl::glUniform1ui(location, 0);
    }

    {
      assert(map_->tileset_->tile_size_x_);
      auto location = gl::glGetUniformLocation(program_, "tile_count_x");
      gl::glUniform1ui(location, map_->tileset_->tile_size_x_);
    }

    {
      assert(map_->tileset_->tile_size_y_);
      auto location = gl::glGetUniformLocation(program_, "tile_count_y");
      gl::glUniform1ui(location, map_->tileset_->tile_size_y_);
    }

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

    quad_.draw();
  }

private:
  alignas(sizeof(float) * 3) struct TileVertex
  {
    float x, y, z;
  };

  struct Quad
  {
    Quad()
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

    auto draw() -> void
    {
      gl::glBindVertexArray(vao_);
      gl::glDrawArrays(gl::GL_TRIANGLE_STRIP, 0, 3 * 2);
      gl::glBindVertexArray(0);
    }

    VertexArray vao_;
    Buffer vbo_;
    Buffer veo_;
    std::vector<TileVertex> vertices_ = {
      { 0, 0, 0.0 },
      { 1.0, 0.0, 0.0 },
      { 0.0, 1.0, 0.0 },
      { 1.0, 1.0, 0.0 },
    };

    std::vector<gl::GLuint> indices_ = { 0, 1, 2, 3 };

  } quad_;

  // TODO: camera
  // TODO: world definition
  std::shared_ptr<TiledMap> map_;

  unsigned screen_width;
  unsigned screen_height;

  Program program_;
};
}