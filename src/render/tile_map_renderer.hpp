#include <render/tile_renderer.hpp>

namespace render {
class TileMapRenderer
{
public:
  TileMapRenderer() = delete;
  explicit TileMapRenderer(TileRenderer& renderer);

  auto add_map([[maybe_unused]] const std::string& map_name,
               std::shared_ptr<TiledMap> map) -> void;

  auto activate_map(const std::string& map_name);
  auto render() -> void;

private:
  TileRenderer& renderer_;

  std::shared_ptr<TiledMap> map_;
};

} // namespace render