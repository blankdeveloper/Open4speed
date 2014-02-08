//----------------------------------------------------------------------------------------
/**
 * \file       pngloader.h
 * \author     Vonasek Lubos
 * \date       2014/01/05
 * \brief      Loading and storing textures from PNG
*/
//----------------------------------------------------------------------------------------

#ifndef PNGLOADER_H
#define PNGLOADER_H

#include "stdafx.h"

/**
 * @brief bmp24 loads texture from png file
 * @param filename is name of file
 * @return texture instance
 */
Texture* loadPNG(const char* filename);

/**
 * @brief writeImage writes image into file
 * @param filename is filename of output
 * @param width is image width
 * @param height is image height
 * @param buffer is image data
 * @return 0 if successed
 */
int writeImage(char* filename, int width, int height, GLubyte *buffer);

#endif // PNGLOADER_H
