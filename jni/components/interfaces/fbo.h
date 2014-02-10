//----------------------------------------------------------------------------------------
/**
 * \file       fbo.h
 * \author     Vonasek Lubos
 * \date       2014/02/10
 * \brief      Framebuffer object interface
*/
//----------------------------------------------------------------------------------------


#ifndef FBO_H
#define FBO_H

#include "stdafx.h"

class fbo
{
public:

    unsigned int res;                  ///< Resolution of texture
    unsigned int width;                ///< Image width
    unsigned int height;               ///< Image height

    /**
     * @brief bind binds FBO
     */
    virtual void bindFBO() = 0;

    /**
     * @brief bind binds texture
     */
    virtual void bindTexture() = 0;

    /**
     * @brief clear clears fragmet/depth buffer
     * @param colors true to clear both, false to clear only depth buffer
     */
    virtual void clear(bool colors) = 0;

    /**
     * @brief drawOnScreen draws FBO on screen
     * @param screen_shader is shader for screen drawing
     */
    virtual void drawOnScreen(shader* screen_shader) = 0;
};
#endif // FBO_H
