#pragma once

namespace render {
namespace interfaces {
/**
 * @brief Abstract representation of surface that can be rendered onto
 *
 */
class IRenderable
{
public:
  virtual auto swap_buffers() -> void = 0;
};
} // namespace render::interfaces
} // namespace bm