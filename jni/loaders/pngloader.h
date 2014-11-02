//----------------------------------------------------------------------------------------
/**
 * \file       pngloader.h
 * \author     Vonasek Lubos
 * \date       2014/11/01
 * \brief      Loading and storing textures from PNG
*/
//----------------------------------------------------------------------------------------

#ifndef PNGLOADER_H
#define PNGLOADER_H

#include <string>
#include "interfaces/texture.h"

/**
 * @brief bmp24 loads texture from png file
 * @param filename is name of file
 * @return texture instance
 */
Texture loadPNG(std::string filename);

#endif // PNGLOADER_H
