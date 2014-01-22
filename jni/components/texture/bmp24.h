//----------------------------------------------------------------------------------------
/**
 * \file       bmp24.h
 * \author     Vonasek Lubos
 * \date       2014/01/05
 * \brief      Loading and storing textures from 24bit BMP
*/
//----------------------------------------------------------------------------------------

#ifndef BMP24_H
#define BMP24_H

#include "stdafx.h"

/**
 * @brief The bmp24 class is bmp2d loader
 */
class bmp24 : public texture {
public:

    /**
     * @brief destruct removes texture from memory is there is no more instance
     */
    void pointerDecrease();

    /**
     * @brief bmp24 loads texture from bmp24 file
     * @param filename is name of file
     * @param alpha is amount of blending
     */
    bmp24(const char* filename, float alpha);

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

#endif // BMP24_H
