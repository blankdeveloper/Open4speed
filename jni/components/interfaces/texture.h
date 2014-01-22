//----------------------------------------------------------------------------------------
/**
 * \file       texture.h
 * \author     Vonasek Lubos
* \date       2014/01/05
 * \brief      Loading and storing textures
*/
//----------------------------------------------------------------------------------------

#ifndef TEXTURE_H
#define TEXTURE_H

#include "stdafx.h"

/**
 * @brief The texture interface
 */
class texture {
public:
    float alpha;            ///< Texture alpha value for blending
    bool transparent;       ///< Texture transparency
    int instanceCount;      ///< Amount of instances
    int twidth, theight;    ///< Image dimensions
    char texturename[256];  ///< Texture filename
    unsigned int textureID; ///< Texture id

    /**
     * @brief destruct removes texture from memory is there is no more instance
     */
    virtual void pointerDecrease() = 0;

    /**
     * @brief apply applies current texture
     */
    virtual void apply() = 0;

    /**
     * @brief setFrame set frame of animation
     * @param frame is index of frame
     */
    virtual void setFrame(int frame) = 0;
};

#endif // TEXTURE_H
