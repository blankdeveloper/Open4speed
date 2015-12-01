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

    data = texture.data;
    twidth = texture.width;
    theight = texture.height;
    transparent = texture.hasAlpha;
    animated = false;
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
    int i, w, h;
    unsigned char* d;
    if (currentFrame == -1) {
        d = data;
        i = transparent ? 4 : 3;
        w = twidth;
        h = theight;
    } else {
        d = ((stexture*)anim[currentFrame])->data;
        i = anim[currentFrame]->transparent ? 4 : 3;
        w = anim[currentFrame]->twidth;
        h = anim[currentFrame]->theight;
    }
    s *= w;
    t *= h;
    i *= ((int)s + (int)t * w);
    buffer[mem] = {d[i + 0], d[i + 1], d[i + 2]};
}
