#pragma once

#include <array>
#include <cstring>
#include <filesystem>
#include <memory>
#include <spdlog/spdlog.h>
#include <vector>

#include <FreeImage.h>
#include <glbinding/gl/gl.h>

#include <render/resource.hpp>
#include <render/tileset.hpp>

namespace render {
auto
load_shader(gl::GLenum type, const std::string& code) -> Shader;

auto
link_program(const std::vector<Shader>& shaders) -> Program;

} // namespace render