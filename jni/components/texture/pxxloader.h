//----------------------------------------------------------------------------------------
/**
 * \file       pxxloader.h
 * \author     Vonasek Lubos
 * \date       2014/01/05
 * \brief      Loading and storing sequences of PNG
*/
//----------------------------------------------------------------------------------------

#ifndef PXXLOADER_H
#define PXXLOADER_H

#include "stdafx.h"

/**
 * @brief The pxxloader class is png sequence loader
 */
class pxxloader : public texture {
public:

    std::vector<gltexture*> anim;   ///< Animation images
    unsigned int currentFrame;      ///< Current image
    unsigned int currentMultiFrame; ///< Replying frames state
    unsigned int multiFrame;        ///< Replying frames amount

    /**
     * @brief destruct removes texture from memory is there is no more instance
     */
    void pointerDecrease();

    /**
     * @brief bmp24 loads texture from png file
     * @param filename is name of file
     * @param alpha is amount of blending
     */
    pxxloader(const char* filename, float alpha);

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

#endif // PXXLOADER_H
