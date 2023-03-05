#pragma once

#include <memory>
#include <string>
#include <vector>

#include <glbinding/gl/gl.h>
#include <glm/glm.hpp>

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
  TileRenderer(Program&& program);

  auto add_map([[maybe_unused]] const std::string& map_name,
               std::shared_ptr<TiledMap> map) -> void;

  auto render() -> void;

private:
  auto get_screen_size() const -> glm::vec2;
  auto set_projection_matrix(float min_x, float min_y, float max_x, float max_y)
    -> void;

  auto draw_quad(float x1, float y1, float x2, float y2, unsigned tile_index)
    -> void;

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
  std::shared_ptr<TiledMap> map_;

  unsigned screen_width;
  unsigned screen_height;

  Program program_;
};
}