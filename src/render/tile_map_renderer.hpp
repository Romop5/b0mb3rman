#include <render/tile_renderer.hpp>

namespace render {
class TileMapRenderer
{
public:
  TileMapRenderer() = delete;
  explicit TileMapRenderer(TileRenderer& renderer);

  auto render(const TiledMap& map) -> void;
  auto get_tile_size(const TiledMap& map) -> glm::vec2;

private:
  TileRenderer& renderer_;
};

} // namespace render