#include <cassert>
#include <cctype>
#include <fmt/format.h>
#include <regex>
#include <utils/color.hpp>

using namespace utils;

namespace {

auto
ascii_to_hex(char c) -> std::uint8_t
{
  assert(std::isxdigit(c));
  const auto lower_c = std::tolower(c);
  // assert(std::islower(lower_c));

  if (lower_c >= '0' && lower_c <= '9') {
    return lower_c - '0';
  } else {
    return (lower_c - 'a') + 10;
  }
}
} // namespace

Color::Color(std::string hex_color)
{
  if (hex_color.size() > 0) {

    const auto starts_with_hash = hex_color.at(0) == '#';
    const auto hex_part = starts_with_hash ? hex_color.substr(1) : hex_color;

    if (std::regex_match(hex_part, std::regex("[0-9a-f]{6}"))) {
      r = (ascii_to_hex(hex_part[0]) << 4) + ascii_to_hex(hex_part[1]);
      g = (ascii_to_hex(hex_part[2]) << 4) + ascii_to_hex(hex_part[3]);
      b = (ascii_to_hex(hex_part[4]) << 4) + ascii_to_hex(hex_part[5]);
      return;
    } else if (std::regex_match(hex_part, std::regex("[0-9a-f]{8}"))) {
      r = (ascii_to_hex(hex_part[0]) << 4) + ascii_to_hex(hex_part[1]);
      g = (ascii_to_hex(hex_part[2]) << 4) + ascii_to_hex(hex_part[3]);
      b = (ascii_to_hex(hex_part[4]) << 4) + ascii_to_hex(hex_part[5]);
      a = (ascii_to_hex(hex_part[6]) << 4) + ascii_to_hex(hex_part[7]);
      return;
    }
  }
  throw std::runtime_error(fmt::format(
    "Expected '#rrggbbaa' or 'rrggbbaa in Color. Got: '{}'", hex_color));
}
Color::Color(std::uint64_t rgba)
{
  r = ((rgba >> 24) & 0xFF);
  g = ((rgba >> 16) & 0xFF);
  b = ((rgba >> 8) & 0xFF);
  a = ((rgba >> 0) & 0xFF);
}
Color::Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
  : r{ r }
  , g{ g }
  , b{ b }
  , a{ a }
{}
Color::Color(float r, float g, float b, float a)
  : r{ r / 255.0 }
  , g{ g / 255.0 }
  , b{ b / 255.0 }
  , a{ a / 255.0 }
{}

auto
Color::operator==(const Color& other) -> bool
{
  return r == other.r && g == other.g && b == other.b && a == other.a;
}