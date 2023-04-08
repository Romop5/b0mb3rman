#include <render/loader.hpp>

#include <memory>
#include <vector>

#include <FreeImage.h>
#include <glbinding/gl/gl.h>
#include <spdlog/spdlog.h>

static struct FreeImageInitializerGuard
{
  FreeImageInitializerGuard() { FreeImage_Initialise(); }
  ~FreeImageInitializerGuard() { FreeImage_DeInitialise(); }
} guard;

using namespace render;

auto
render::load_texture_from_file(const std::filesystem::path& path,
                               std::optional<utils::Color> alpha_color)
  -> Texture
try {
  spdlog::trace("load_texture_from_file: '{}'", path.c_str());
  if (not std::filesystem::exists(path)) {
    throw std::runtime_error(fmt::format("Missing file {}", path.c_str()));
  }

  // Source:
  // https://stackoverflow.com/questions/19606736/loading-an-image-with-freeimage

  FREE_IMAGE_FORMAT formato = FreeImage_GetFileType(path.c_str(), 0);
  formato = formato == FIF_UNKNOWN ? FreeImage_GetFIFFromFilename(path.c_str())
                                   : formato;
  if (formato == FIF_UNKNOWN) {
    if (FreeImage_GetFIFCount() == 0) {
      throw std::runtime_error("Internal error: FreeImage supports 0 formats");
    }

    unsigned formats_count = 0;
    for (unsigned i = 0; i < FreeImage_GetFIFCount(); i++) {
      if (FreeImage_FIFSupportsReading((FREE_IMAGE_FORMAT)i)) {
        formats_count++;
        spdlog::info("FreeImage supported format: {} (extensions: {})",
                     FreeImage_GetFIFDescription((FREE_IMAGE_FORMAT)i),
                     FreeImage_GetFIFExtensionList((FREE_IMAGE_FORMAT)i));
      }
    }
    spdlog::info("FreeImage: described formats: {}", formats_count);

    throw std::runtime_error("Uknown type of texture");
  }

  spdlog::info("Loading format: {}", FreeImage_GetFIFDescription(formato));

  FIBITMAP* imagen = FreeImage_Load(formato, path.c_str());

  if (not imagen) {
    throw std::runtime_error("Failed to load image via FreeImage");
  }

  FIBITMAP* temp = imagen;
  imagen = FreeImage_ConvertTo32Bits(imagen);

  const auto width_ = FreeImage_GetWidth(imagen);
  const auto height_ = FreeImage_GetHeight(imagen);

  char* image_data = (char*)FreeImage_GetBits(imagen);

  if (width_ == 0 || height_ == 0) {
    throw std::runtime_error("Invalid image: one of dimensions is 0!");
  }

  /**
   * @brief Tigtly-packed RGBA (8-bit per channel)
   *
   */
  union PixelData
  {
    gl::GLubyte bytes[4];
    struct
    {
      gl::GLubyte r, g, b, a;
    };
  };
  static_assert(sizeof(PixelData) == 4, "PixelData must be tightly-packed");

  auto texture_data =
    std::unique_ptr<PixelData[]>{ new PixelData[width_ * height_] };

  PixelData* gl_texture_data = texture_data.get();

  /// Should invert texture's Y row (due to OpenGL's (0,0) being in bottom-down
  /// corner)
  bool invert_texture_v_axis = false;

  for (int r = 0; r < height_; r++) {
    for (int c = 0; c < width_; c++) {

      const auto row = invert_texture_v_axis ? height_ - r - 1 : r;
      const auto j_out = row * width_ + c;
      const auto j_in = r * width_ + c;

      // Assumes data stored as BGRA?
      gl_texture_data[j_out].r = image_data[j_in * 4 + 2];
      gl_texture_data[j_out].g = image_data[j_in * 4 + 1];
      gl_texture_data[j_out].b = image_data[j_in * 4 + 0];
      gl_texture_data[j_out].a = image_data[j_in * 4 + 3];

      if (alpha_color) {
        const auto& color = *alpha_color;
        auto& pixel = gl_texture_data[j_out];
        if (utils::Color(pixel.r, pixel.g, pixel.b) == color) {
          pixel.a = 0;
        }
      }
    }
  }

  FreeImage_Unload(temp);
  FreeImage_Unload(imagen);

  auto result = create_texture();
  gl::GLuint tex = result;

  gl::glBindTexture(gl::GL_TEXTURE_2D, tex);
  gl::glTexImage2D(gl::GL_TEXTURE_2D,
                   0,
                   gl::GL_RGBA,
                   width_,
                   height_,
                   0,
                   gl::GL_RGBA,
                   gl::GL_UNSIGNED_BYTE,
                   (gl::GLvoid*)gl_texture_data);

  gl::glTexParameteri(
    gl::GL_TEXTURE_2D, gl::GL_TEXTURE_WRAP_S, gl::GL_CLAMP_TO_EDGE);
  gl::glTexParameteri(
    gl::GL_TEXTURE_2D, gl::GL_TEXTURE_WRAP_T, gl::GL_CLAMP_TO_EDGE);
  gl::glTexParameteri(
    gl::GL_TEXTURE_2D, gl::GL_TEXTURE_MIN_FILTER, gl::GL_NEAREST);
  gl::glTexParameteri(
    gl::GL_TEXTURE_2D, gl::GL_TEXTURE_MAG_FILTER, gl::GL_NEAREST);

  return result;
} catch (const std::exception& e) {
  throw std::runtime_error(fmt::format("{}: {}", path.c_str(), e.what()));
}