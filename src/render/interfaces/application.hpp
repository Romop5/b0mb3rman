#pragma once

namespace render {
namespace interfaces {

class IApplication
{
public:
  virtual auto run() -> void = 0;
  virtual auto stop() -> void = 0;
};
} // namespace render::interfaces
} // namespace bm