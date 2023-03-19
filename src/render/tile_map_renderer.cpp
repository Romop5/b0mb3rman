#include <render/tile_map_renderer.hpp>

using namespace render;

TileMapRenderer::TileMapRenderer(TileRenderer& renderer)
  : renderer_{ renderer }
{
}

auto
TileMapRenderer::add_map([[maybe_unused]] const std::string& map_name,
                         std::shared_ptr<TiledMap> map) -> void
{
  // TODO: use dictionary for maps
  map_ = std::move(map);
}

auto
TileMapRenderer::activate_map(const std::string& map_name)
{
}

auto
TileMapRenderer::render() -> void
{
  assert(map_);

  renderer_.bind_tileset(*map_->tileset_);

  const auto tile_size = get_tile_size();
  const auto tile_width = tile_size[0];
  const auto tile_height = tile_size[1];

  for (size_t y = 0; y < map_->count_y; y++) {
    for (size_t x = 0; x < map_->count_x; x++) {
      // Map (x,y) to <0, width*height)
      const auto tile_position_index = y * map_->count_x + x;
      const auto tile_texture_index =
        map_->tile_indices_.at(tile_position_index);

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
TileMapRenderer::get_tile_size() -> glm::vec2
{
  const auto screen_size = renderer_.get_screen_size();

  const auto tile_width = screen_size[0] / map_->count_x;
  const auto tile_height = screen_size[1] / map_->count_y;
  return glm::vec2(tile_width, tile_height);
}