///----------------------------------------------------------------------------------------
/**
 * \file       stexture.h
 * \author     Vonasek Lubos
 * \date       2015/12/01
 * \brief      Loading and storing textures
**/
///----------------------------------------------------------------------------------------


#ifndef stexture_H
#define stexture_H

#include "interfaces/texture.h"

// Struct for color of pixel r,g,b red, green, blue values
struct Color {
    unsigned char r,g,b;
};

class stexture : public texture {
public:

    bool animated;                  ///< True if it is texture sequence
    std::vector<texture*> anim;     ///< Animation images
    unsigned int currentFrame;      ///< Current image
    unsigned int currentMultiFrame; ///< Replying frames state
    unsigned int multiFrame;        ///< Replying frames amount
    Color* data;                    ///< Texture data


    /**
     * @brief destruct removes texture from memory
     */
    ~stexture();

    /**
     * @brief stexture creates animated empty texture instance
     * @param anim is an image sequence
     * @param alpha is amount of blending
     */
    stexture(std::vector<texture*> anim, float alpha);

    /**
     * @brief stexture creates texture from raster data
     * @param texture is texture raster instance
     * @param alpha is amount of blending
     */
    stexture(Texture texture, float alpha);

    /**
     * @brief apply applies current texture
     */
    void apply();

    /**
     * @brief setFrame set frame of animation
     * @param frame is index of frame
     */
    void setFrame(int frame);

    /**
     * @brief setPixel draws pixel into buffer from texture
     * @param buffer is buffer for writing
     * @param mem is position in buffer to write
     * @param s is position x in texture
     * @param t is position y in texture
     */
    void setPixel(Color* buffer, int mem, double s, double t);
};

#endif // stexture_H
