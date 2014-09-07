//----------------------------------------------------------------------------------------
/**
 * \file       rgb.h
 * \author     Vonasek Lubos
 * \date       2014/02/09
 * \brief      Loading and storing textures
*/
//----------------------------------------------------------------------------------------

#ifndef RGB_H
#define RGB_H

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
Texture createRGB(int width, int height, float r, float g, float b);

#endif // RGB_H
