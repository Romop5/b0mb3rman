#pragma once

#include <memory>
#include <string>
#include <vector>

#include <glbinding/gl/gl.h>
#include <glm/glm.hpp>

#include <render/tile_program.hpp>
#include <render/tiled_map.hpp>
#include <render/tileset.hpp>
#include <render/viewport.hpp>

namespace render {
/**
 * @brief Renders a tile world
 *
 */
class TileRenderer
{
public:
  TileRenderer(const Viewport& viewport, Program&& program);

  auto bind_tileset(const Tileset& tileset) -> void;
  auto draw_quad(float x1, float y1, float x2, float y2, unsigned tile_index)
    -> void;
  auto draw_quad(const glm::vec2& position,
                 const glm::vec2& size,
                 unsigned tile_index) -> void;

  auto set_projection_matrix(float min_x, float min_y, float max_x, float max_y)
    -> void;

  auto get_viewport() const -> const Viewport&;

private:
  struct TileVertex
  {
    float x, y, z;
  };

  static_assert(sizeof(TileVertex) == sizeof(float) * 3,
                "TileVertex must be tightly-packed");

  struct Quad
  {
    Quad();

    auto draw() -> void;

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

  unsigned screen_width;
  unsigned screen_height;

  Program program_;
  const Viewport& viewport_;
};
}