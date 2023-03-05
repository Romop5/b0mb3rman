#include <utils/io.hpp>

using namespace utils;

auto
utils::read_text_file(const std::filesystem::path& path) -> std::string
{
  if (!std::filesystem::exists(path)) {
    throw std::runtime_error(
      fmt::format("read_text_file: non existing path '{}'", path.c_str()));
  }

  try {
    // Open file (with exceptions on)
    std::ifstream file(path);
    constexpr auto all_bits =
      std::ifstream::failbit | std::ifstream::badbit | std::ifstream::eofbit;
    file.exceptions(all_bits);

    // Determine file size
    file.seekg(0, std::ios_base::end);
    const auto file_size = file.tellg();
    file.seekg(0, std::ios_base::beg);

    std::vector<char> file_data(file_size, '\0');
    file.read(file_data.data(), file_size);
    auto end_of_file_iterator =
      std::find(file_data.begin(), file_data.end(), '\0');

    return std::string{ file_data.begin(), end_of_file_iterator };
  } catch (const std::exception& e) {
    throw std::runtime_error(
      fmt::format("read_text_file: '{}': {}", path.c_str(), e.what()));
  }
}