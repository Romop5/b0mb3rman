#pragma once

namespace render {
namespace interfaces {

class IRenderableObserver
{
public:
  virtual auto on_viewport_change(float x, float y, float width, float height)
    -> void = 0;
};
} // namespace render::interfaces
} // namespace bm