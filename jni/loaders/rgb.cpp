//----------------------------------------------------------------------------------------
/**
 * \file       rgb.cpp
 * \author     Vonasek Lubos
 * \date       2014/11/01
 * \brief      Loading and storing textures
*/
//----------------------------------------------------------------------------------------

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
