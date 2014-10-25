//----------------------------------------------------------------------------------------
/**
 * \file       fbo.h
 * \author     Vonasek Lubos
 * \date       2014/02/11
 * \brief      Framebuffer object interface
*/
//----------------------------------------------------------------------------------------


#ifndef FBO_H
#define FBO_H

#include "interfaces/shader.h"

class fbo
{
public:

    unsigned int width;                ///< Image width
    unsigned int height;               ///< Image height
    bool complete;                     ///< Information if fbo is complete

    virtual ~fbo() {}

    /**
     * @brief bindFBO binds FBO
     */
    virtual void bindFBO() = 0;

    /**
     * @brief bindTexture binds texture
     */
    virtual void bindTexture() = 0;

    /**
     * @brief clear clears stencil/depth buffer
     */
    virtual void clear() = 0;

    /**
     * @brief drawOnScreen draws FBO on screen
     * @param screen_shader is shader for screen drawing
     */
    virtual void drawOnScreen(shader* screen_shader) = 0;

    /**
     * @brief unbindFBO unbinds FBO
     */
    virtual void unbindFBO() = 0;
};
#endif // FBO_H
