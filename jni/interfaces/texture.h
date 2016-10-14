///----------------------------------------------------------------------------------------
/**
 * \file       texture.h
 * \author     Vonasek Lubos
 * \date       2014/12/30
 * \brief      Loading and storing textures
**/
///----------------------------------------------------------------------------------------

#ifndef TEXTURE_H
#define TEXTURE_H

#include "files/extfile.h"
#include "files/zipfile.h"
#include <vector>

struct Texture
{
    int width;
    int height;
    unsigned char* data;
    bool hasAlpha;
};

/**
 * @brief The texture interface
 */
class texture
{
public:
    bool transparent;       ///< Texture transparency
    int twidth, theight;    ///< Image dimensions
    unsigned int textureID; ///< Texture id

    bool animated;                  ///< True if it is texture sequence
    std::vector<texture*> anim;     ///< Animation images
    unsigned int currentFrame;      ///< Current image
    unsigned int currentMultiFrame; ///< Replying frames state
    unsigned int multiFrame;        ///< Replying frames amount

    virtual ~texture() {}

    /**
     * @brief apply applies current texture
     */
    virtual void apply() = 0;

    /**
     * @brief setFrame set frame of animation
     * @param frame is index of frame
     */
    virtual void setFrame(int frame) = 0;

    /**
     * @brief rgb creates texture from color
     * @param width is image width
     * @param height is image height
     * @param r is red color value
     * @param g is green color value
     * @param b is blue color value
     * @return texture instance
     */
    static inline Texture createRGB(int width, int height, float r, float g, float b)
    {

        /// create color pixel raster
        Texture texture;
        texture.data = new unsigned char[width * height * 4];
        int index = 0;
        for (int x = 0; x < width; x++)
            for (int y = 0; y < height; y++)
            {
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

    /**
     * @brief pngloader loads texture from png file
     * @param filename is name of file
     * @return texture instance
     */
    static inline Texture loadPNG(file* f)
    {
      Texture texture;
      unsigned int sig_read = 0;

      /// init PNG library
      png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
      png_infop info_ptr = png_create_info_struct(png_ptr);
      setjmp(png_jmpbuf(png_ptr));
      f->accessStatic();
      if (f->isArchive())
        png_set_read_fn(png_ptr, NULL, png_read_zip);
      else
        png_set_read_fn(png_ptr, NULL, png_read_ext);
      png_set_sig_bytes(png_ptr, sig_read);
      png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16, NULL);
      int bit_depth, color_type, interlace_type;
      png_uint_32 width, height;
      png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, NULL, NULL);

      /// get PNG type
      texture.hasAlpha = true;
      switch (color_type)
      {
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
      for (unsigned int i = 0; i < height; i++)
          memcpy(texture.data+(row_bytes * (height-1-i)), row_pointers[i], row_bytes);

      /* Clean up after the read,
       * and free any memory allocated */
      png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
      delete f;

      texture.width = width;
      texture.height = height;
      return texture;
    }
};

#endif // TEXTURE_H
