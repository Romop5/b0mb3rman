#pragma once

#include <filesystem>
#include <optional>
#include <string>

#include <render/resource.hpp>
#include <utils/color.hpp>

namespace render {
auto
load_texture_from_file(const std::filesystem::path& path,
                       std::optional<utils::Color> alpha_color) -> Texture;

} // namespace render