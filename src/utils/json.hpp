#pragma once

#include <filesystem>
#include <fmt/format.h>
#include <fstream>
#include <nlohmann/json.hpp>

namespace utils {

auto
read_json(const std::filesystem::path& path) -> nlohmann::json;

} // namespace utils