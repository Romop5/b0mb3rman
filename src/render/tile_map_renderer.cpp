#include <cassert>
#include <render/tile_map_renderer.hpp>

using namespace render;

TileMapRenderer::TileMapRenderer(TileRenderer& renderer)
  : renderer_{ renderer }
{}

auto
TileMapRenderer::render(const TiledMap& map) -> void
{
  renderer_.bind_tileset(*map.tileset_);

  const auto tile_size = get_tile_size(map);
  const auto tile_width = tile_size[0];
  const auto tile_height = tile_size[1];

  for (size_t y = 0; y < map.count_y; y++) {
    for (size_t x = 0; x < map.count_x; x++) {
      // Map (x,y) to <0, width*height)
      const auto tile_position_index = y * map.count_x + x;
      const auto tile_texture_index = map.tile_indices_.at(tile_position_index);

      const auto start_x = tile_width * x;
      const auto start_y = tile_height * y;

      renderer_.draw_quad(start_x,
                          start_y,
                          start_x + tile_width,
                          start_y + tile_height,
                          tile_texture_index);
    }
  }
}

auto
TileMapRenderer::get_tile_size(const TiledMap& map) -> glm::vec2
{
  const auto screen_size = renderer_.get_viewport().get_size();

  const auto tile_width = screen_size[0] / map.count_x;
  const auto tile_height = screen_size[1] / map.count_y;
  return glm::vec2(tile_width, tile_height);
}