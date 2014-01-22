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
 * @brief The pngloader class is png texture loader
 */
class pngloader : public texture {
public:

    /**
     * @brief destruct removes texture from memory is there is no more instance
     */
    void pointerDecrease();

    /**
     * @brief bmp24 loads texture from png file
     * @param filename is name of file
     * @param alpha is amount of blending
     */
    pngloader(const char* filename, float alpha);

    /**
     * @brief apply applies current texture
     */
    void apply();

    /**
     * @brief setFrame set frame of animation
     * @param frame is index of frame
     */
    void setFrame(int frame);
};

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
