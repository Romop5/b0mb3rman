#pragma once

namespace render {
namespace interfaces {

class IInputListener
{
public:
  virtual auto on_key_callback(int key, int scancode, int action, int mods)
    -> void = 0;
};
} // namespace render::interfaces
} // namespace bm