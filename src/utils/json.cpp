#include <spdlog/spdlog.h>
#include <utils/json.hpp>

using namespace utils;

auto
utils::read_json(const std::filesystem::path& path) -> nlohmann::json
{
  spdlog::trace("utils::read_json: {}", path.c_str());

  if (!std::filesystem::exists(path)) {
    throw std::runtime_error(
      fmt::format("read_json: non existing path '{}'", path.c_str()));
  }

  try {
    std::ifstream file(path);
    return nlohmann::json::parse(file);
  } catch (const std::exception& e) {
    throw std::runtime_error(
      fmt::format("read_json: '{}': {}", path.c_str(), e.what()));
  }
}
