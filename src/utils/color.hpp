#pragma once

#include <cstdint>
#include <string>

namespace utils {

struct Color
{
public:
  explicit Color(std::string hex_color);
  explicit Color(std::uint64_t rgba);
  explicit Color(unsigned char r,
                 unsigned char g,
                 unsigned char b,
                 unsigned char a = 255);
  explicit Color(float r, float g, float b, float a = 1.0);

  auto operator==(const Color& other) -> bool;

  unsigned char r{ 255 }, g{ 255 }, b{ 255 }, a{ 255 };
};
} // namespace utils