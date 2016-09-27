///----------------------------------------------------------------------------------------
/**
 * \file       gltexture.h
 * \author     Vonasek Lubos
 * \date       2014/12/30
 * \brief      Loading and storing textures in OpenGL
**/
///----------------------------------------------------------------------------------------


#ifndef GLTEXTURE_H
#define GLTEXTURE_H

#include "interfaces/texture.h"

class gltexture : public texture {
public:

    bool animated;                  ///< True if it is texture sequence
    std::vector<texture*> anim;     ///< Animation images
    unsigned int currentFrame;      ///< Current image
    unsigned int currentMultiFrame; ///< Replying frames state
    unsigned int multiFrame;        ///< Replying frames amount
    ColorRGB* dataRGB;              ///< Texture data
    ColorRGBA* dataRGBA;            ///< Texture data


    /**
     * @brief destruct removes texture from memory
     */
    ~gltexture();

    /**
     * @brief gltexture creates animated empty texture instance
     * @param anim is an image sequence
     */
    gltexture(std::vector<texture*> anim);

    /**
     * @brief gltexture creates texture from raster data
     * @param texture is texture raster instance
     */
    gltexture(Texture texture);

    /**
     * @brief apply applies current texture
     */
    void apply();

    ColorRGBA getPixel(double s, double t);

    /**
     * @brief setFrame set frame of animation
     * @param frame is index of frame
     */
    void setFrame(int frame);
};

#endif // GLTEXTURE_H
