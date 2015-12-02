///----------------------------------------------------------------------------------------
/**
 * \file       stexture.cpp
 * \author     Vonasek Lubos
 * \date       2015/12/01
 * \brief      Loading and storing textures
**/
///----------------------------------------------------------------------------------------

#include <cstring>
#include "renderers/simple/stexture.h"

/**
 * @brief destruct removes texture from memory
 */
stexture::~stexture() {
    if (!animated)
        delete[] data;
    else
        for (int i = anim.size() - 1; i >= 0; i--)
            delete anim[i];
}

/**
 * @brief stexture creates animated empty texture instance
 * @param anim is an image sequence
 * @param alpha is amount of blending
 */
stexture::stexture(std::vector<texture*> anim, float alpha) {
    this->alpha = alpha;
    this->anim = anim;
    transparent = true;
    animated = true;

    /// set animation speed
    multiFrame = anim.size() / 50;
    if (anim.size() < 10)
        multiFrame = 3;
    if (multiFrame < 1)
        multiFrame = 1;

    currentFrame = 0;
    currentMultiFrame = 0;
}

/**
 * @brief stexture creates texture from raster data
 * @param texture is texture raster instance
 * @param alpha is amount of blending
 */
stexture::stexture(Texture texture, float alpha) {
    twidth = texture.width;
    theight = texture.height;
    transparent = texture.hasAlpha;
    animated = false;

    int index = 0;
    int size = twidth * theight;
    data = new Color[size];
    for (int i = 0 ; i < size; i++) {
        data[i].r = texture.data[index++];
        data[i].g = texture.data[index++];
        data[i].b = texture.data[index++];
        if (transparent)
            index++;
    }
    delete[] texture.data;
}

/**
 * @brief apply applies current texture
 */
void stexture::apply() {
    if (animated) {
        /// update animation
        currentMultiFrame++;
        if (currentMultiFrame == multiFrame) {
            currentMultiFrame = 0;
            currentFrame++;
        }
        if (currentFrame == anim.size())
            currentFrame = 0;
    } else
        currentFrame = -1;
}

/**
 * @brief setFrame set frame of animation
 * @param frame is index of frame
 */
void stexture::setFrame(int frame) {
    currentFrame = frame;
}

/**
 * @brief setPixel draws pixel into buffer from texture
 * @param buffer is buffer for writing
 * @param mem is position in buffer to write
 * @param s is position x in texture
 * @param t is position y in texture
 */
void stexture::setPixel(Color* buffer, int mem, double s, double t) {
    s -= (int)s;
    t -= (int)t;
    if (s < 0)
        s++;
    if (t < 0)
        t++;
    if (currentFrame == -1) {
        s *= twidth - 1;
        t *= theight - 1;
        buffer[mem] = data[(int)s + (int)t * twidth];
    } else {
        s *= anim[currentFrame]->twidth - 1;
        t *= anim[currentFrame]->theight - 1;
        buffer[mem] = ((stexture*)anim[currentFrame])->data[(int)s + (int)t * anim[currentFrame]->twidth];
    }
}
