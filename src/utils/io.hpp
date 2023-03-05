#pragma once

#include <filesystem>
#include <fmt/format.h>
#include <fstream>
#include <nlohmann/json.hpp>

namespace utils {

auto
read_text_file(const std::filesystem::path& path) -> std::string;

} // namespace utils