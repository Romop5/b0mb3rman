#pragma once

#include <filesystem>

#include <render/resource.hpp>

namespace render {
auto
load_texture_from_file(const std::filesystem::path& path) -> Texture;

} // namespace render