//----------------------------------------------------------------------------------------
/**
 * \file       rgb.h
 * \author     Vonasek Lubos
 * \date       2014/01/05
 * \brief      Loading and storing textures
*/
//----------------------------------------------------------------------------------------

#ifndef RGB_H
#define RGB_H

#include "stdafx.h"

/**
 * @brief The rgb class
 */
class rgb : public texture {
public:

    /**
     * @brief destruct removes texture from memory is there is no more instance
     */
    void pointerDecrease();

    /**
     * @brief rgb creates texture from color
     * @param width is image width
     * @param height is image height
     * @param r is red color value
     * @param g is green color value
     * @param b is blue color value
     * @param alpha is amount of blending
     */
    rgb(int width, int height, float r, float g, float b, float alpha);

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

#endif // RGB_H
