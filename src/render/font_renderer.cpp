#include <render/font_renderer.hpp>

#include <glbinding/glbinding.h>
#include <render/resource.hpp>
#include <render/tile_program.hpp>
#include <render/viewport.hpp>

#include <utils/opengl.hpp>
#include <utils/raii_helpers.hpp>
#include <utils/singleton.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include <glm/gtx/transform.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

using namespace render;
namespace {
// Taken from
// https://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Text_Rendering_01
const std::string font_rendering_vertex_shader =
  R"(#version 330 core

layout (location = 0) in vec4 coord;
out vec2 texcoord;
uniform mat4 projection;

void main(void) {
  vec4 coord_out = projection * vec4(coord.xy, 0,1);
  gl_Position = coord_out;
  gl_Position.y = gl_Position.y*-1;
  //gl_Position = vec4(coord.xy, 0, 1);
  texcoord = coord.zw;
}
)";

// Taken from
// https://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Text_Rendering_01
const std::string font_rendering_fragment_shader =
  R"(#version 330 core

out vec4 FragColor;
in vec2 texcoord;
uniform sampler2D tex;
uniform vec4 color;

void main(void) {
  FragColor = vec4(1, 1, 1, texture2D(tex, texcoord).r);
  //FragColor = vec4(1, 1, 1, 1);
}
)";

class FreeTypeLibraryHandle : public utils::Singleton<FreeTypeLibraryHandle>
{
public:
  friend utils::Singleton<FreeTypeLibraryHandle>;

  FreeTypeLibraryHandle()
  {
    spdlog::trace("FreeTypeLibraryHandle");
    auto error = ::FT_Init_FreeType(&library_);
    utils::throw_runtime_on_false(error == 0,
                                  "Failed to initialize FreeType library");
  }

  ~FreeTypeLibraryHandle() { FT_Done_FreeType(library_); }

  auto create_face(const std::filesystem::path& font) -> ::FT_Face&
  {
    auto error = ::FT_New_Face(library_, font.c_str(), 0, &face_);

    utils::throw_runtime_on_false(
      error != ::FT_Err_Unknown_File_Format,
      fmt::format("Unknown file format {}", font.c_str()));

    ::FT_Set_Pixel_Sizes(face_, 0, 48);

    return face_;
  }

private:
  ::FT_Library library_;
  ::FT_Face face_;
};
} // namespace

class FontRenderer::FontRendererImpl
{
public:
  FontRendererImpl(const Viewport& viewport,
                   const std::filesystem::path& font_path)
    : viewport_{ viewport }
  {
    face_ = FreeTypeLibraryHandle::get_instance().create_face(font_path);

    // II. Create rendering shdaders
    std::vector<Shader> shaders;
    shaders.emplace_back(
      load_shader(gl::GL_VERTEX_SHADER, font_rendering_vertex_shader));
    shaders.emplace_back(
      load_shader(gl::GL_FRAGMENT_SHADER, font_rendering_fragment_shader));

    program_ = link_program(shaders);
    gl::glUseProgram(program_);
    auto texture_uniform_location = gl::glGetUniformLocation(program_, "tex");
    gl::glUniform1i(texture_uniform_location, 0);
    gl::glUseProgram(0);

    // III. create texture & set params
    texture_ = std::move(render::create_texture());
    gl::glBindTexture(gl::GL_TEXTURE_2D, texture_);

    gl::glTexParameteri(
      gl::GL_TEXTURE_2D, gl::GL_TEXTURE_WRAP_S, gl::GL_CLAMP_TO_EDGE);
    gl::glTexParameteri(
      gl::GL_TEXTURE_2D, gl::GL_TEXTURE_WRAP_T, gl::GL_CLAMP_TO_EDGE);

    gl::glTexParameteri(
      gl::GL_TEXTURE_2D, gl::GL_TEXTURE_MIN_FILTER, gl::GL_LINEAR);
    gl::glTexParameteri(
      gl::GL_TEXTURE_2D, gl::GL_TEXTURE_MAG_FILTER, gl::GL_LINEAR);

    gl::glPixelStorei(gl::GL_UNPACK_ALIGNMENT, 1);

    gl::glBindTexture(gl::GL_TEXTURE_2D, 0);

    // III. create geometry, representing a glyph

    vao_ = render::create_vertex_array();
    vbo_ = render::create_buffer();
    ebo_ = render::create_buffer();

    gl::glBindVertexArray(vao_);
    gl::glEnableVertexAttribArray(0);
    gl::glBindBuffer(gl::GL_ARRAY_BUFFER, vbo_);
    gl::glVertexAttribPointer(0, 4, gl::GL_FLOAT, gl::GL_FALSE, 0, 0);

    gl::glBindBuffer(gl::GL_ELEMENT_ARRAY_BUFFER, ebo_);
    gl::GLuint indices[] = { 0, 1, 2, 3 };
    gl::glBufferData(
      gl::GL_ELEMENT_ARRAY_BUFFER, sizeof indices, indices, gl::GL_STATIC_DRAW);
    gl::glBindVertexArray(0);
    utils::throw_on_opengl_error(
      "OpenGL error after creating VBO for FontRenderer");
  }

  auto compute_text_metrics(const std::string& text) -> glm::vec2
  {

    glm::vec2 size{ 0 };
    for (const auto& c : text) {
      if (::FT_Load_Char(face_, c, FT_LOAD_RENDER))
        continue;

      const auto& g = face_->glyph;
      size += glm::vec2(g->advance.x, g->advance.y) / glm::vec2(64);
    }
    return size;
  }

  auto draw_text(const std::string& text, glm::vec2 origin) -> void
  {
    gl::glUseProgram(program_);
    const auto viewport_min = viewport_.get_origin();
    const auto viewport_max = viewport_.get_origin() + viewport_.get_size();
    auto matrix = glm::ortho(
      viewport_min.x, viewport_max.x, viewport_min.y, viewport_max.y);
    {
      auto location = gl::glGetUniformLocation(program_, "projection");
      gl::glUniformMatrix4fv(location, 1, gl::GL_FALSE, glm::value_ptr(matrix));
    }

    gl::glEnable(gl::GL_BLEND);
    gl::glBlendFunc(gl::GL_SRC_ALPHA, gl::GL_ONE_MINUS_SRC_ALPHA);

    gl::glActiveTexture(gl::GL_TEXTURE0);
    gl::glBindTexture(gl::GL_TEXTURE_2D, texture_);
    gl::glUseProgram(program_);

    glm::vec2 scale{ 1.0 };
    glm::vec2 position = origin;
    for (const auto& c : text) {
      if (::FT_Load_Char(face_, c, FT_LOAD_RENDER))
        continue;

      const auto& g = face_->glyph;
      gl::glTexImage2D(gl::GL_TEXTURE_2D,
                       0,
                       gl::GL_RED,
                       g->bitmap.width,
                       g->bitmap.rows,
                       0,
                       gl::GL_RED,
                       gl::GL_UNSIGNED_BYTE,
                       g->bitmap.buffer);

      float x2 = position.x + g->bitmap_left * scale.x;
      float y2 = -position.y - g->bitmap_top * scale.y;
      float w = g->bitmap.width * scale.x;
      float h = g->bitmap.rows * scale.y;

      gl::GLfloat box[4][4] = {
        { x2, -y2, 0, 1 },
        { x2 + w, -y2, 1, 1 },
        { x2, -y2 - h, 0, 0 },
        { x2 + w, -y2 - h, 1, 0 },
      };

      gl::glBindVertexArray(vao_);
      gl::glBufferData(
        gl::GL_ARRAY_BUFFER, sizeof box, box, gl::GL_DYNAMIC_DRAW);

      gl::glDrawArrays(gl::GL_TRIANGLE_STRIP, 0, 4);

      position += glm::vec2(g->advance.x, g->advance.y) / glm::vec2(64) * scale;
    }
  }

private:
  render::Program program_;
  render::Texture texture_;
  render::VertexArray vao_;
  render::Buffer vbo_;
  render::Buffer ebo_;

  FT_Face face_;

  const Viewport& viewport_;
};

FontRenderer::FontRenderer(const Viewport& viewport,
                           const std::string& font_name)
  : pimpl_{ std::make_unique<FontRendererImpl>(viewport, font_name) }
{
}

FontRenderer::~FontRenderer() = default;

auto
FontRenderer::compute_text_metrics(const std::string& text) -> glm::vec2
{
  return pimpl_->compute_text_metrics(text);
}

auto
FontRenderer::draw_text(const std::string& text, glm::vec2 start) -> void
{
  pimpl_->draw_text(text, start);
}