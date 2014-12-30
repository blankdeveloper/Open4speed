///----------------------------------------------------------------------------------------
/**
 * \file       textures.hpp
 * \author     Vonasek Lubos
 * \date       2014/12/30
 * \brief      Loading textures from PNG and RGB value
**/
///----------------------------------------------------------------------------------------

#include <png.h>
#include "engine/io.h"
#include "interfaces/texture.h"

/**
 * @brief rgb creates texture from color
 * @param width is image width
 * @param height is image height
 * @param r is red color value
 * @param g is green color value
 * @param b is blue color value
 * @return texture instance
 */
Texture createRGB(int width, int height, float r, float g, float b) {

    /// create color pixel raster
    Texture texture;
    texture.data = new unsigned char[width * height * 4];
    int index = 0;
    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++) {
            texture.data[0 + index] = (int)(255 * r);
            texture.data[1 + index] = (int)(255 * g);
            texture.data[2 + index] = (int)(255 * b);
            texture.data[3 + index] = (int)(255 * 1);
            index += 4;
        }

    /// create texture
    texture.width = width;
    texture.height = height;
    texture.hasAlpha = false;
    return texture;
}

#ifdef ZIP_ARCHIVE
zip_file* file;

void png_read(png_structp png_ptr, png_bytep data, png_size_t length) {
  zip_fread(file, data, length);
}
#else
FILE* fp;
void png_read(png_structp png_ptr, png_bytep data, png_size_t length) {
  fread(data, length, 1, fp);
}
#endif

/**
 * @brief pngloader loads texture from png file
 * @param filename is name of file
 * @param alpha is amount of blending
 * @return texture instance
 */
Texture loadPNG(std::string filename) {
  Texture texture;
  unsigned int sig_read = 0;
#ifdef ZIP_ARCHIVE
  file = zip_fopen(getZip(""), filename.c_str(), 0);
#else
  fp = fopen(filename.c_str(), "rb");
#endif

  /// init PNG library
  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  png_infop info_ptr = png_create_info_struct(png_ptr);
  setjmp(png_jmpbuf(png_ptr));
  png_set_read_fn(png_ptr, NULL, png_read);
  png_set_sig_bytes(png_ptr, sig_read);
  png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16, NULL);
  int bit_depth, color_type, interlace_type;
  png_uint_32 width, height;
  png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, NULL, NULL);

  /// get PNG type
  texture.hasAlpha = true;
  switch (color_type) {
      case PNG_COLOR_TYPE_RGBA:
          texture.hasAlpha = true;
          break;
      case PNG_COLOR_TYPE_RGB:
          texture.hasAlpha = false;
          break;
  }

  /// load PNG
  unsigned int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
  texture.data = new unsigned char[row_bytes * height];
  png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);
  for (unsigned int i = 0; i < height; i++) {
      memcpy(texture.data+(row_bytes * (height-1-i)), row_pointers[i], row_bytes);
  }

  /* Clean up after the read,
   * and free any memory allocated */
  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
#ifdef ZIP_ARCHIVE
  zip_fclose(file);
#else
  fclose(fp);
#endif

  texture.width = width;
  texture.height = height;
  return texture;
}
