///----------------------------------------------------------------------------------------
/**
 * \file       glfbo.h
 * \author     Vonasek Lubos
 * \date       2014/12/30
 * \brief      Framebuffer object class for OpenGL
**/
///----------------------------------------------------------------------------------------


#ifndef GLFBO_H
#define GLFBO_H

#include "interfaces/fbo.h"

class glfbo : public fbo
{
public:
    unsigned int* rendertexture;  ///< Texture for color buffer
    unsigned int* fboID;          ///< Frame buffer object id
    unsigned int* rboID;          ///< Render buffer object id

    /**
     * @brief removes all data from memory
     */
    ~glfbo();

    /**
     * @brief constructor init framebuffer and renderbuffer
     * @param width is width of framebuffer
     * @param height is height of framebuffer
     */
    glfbo(int width, int height);

    /**
     * @brief bind binds FBO
     */
    void bindFBO();

    /**
     * @brief bind binds texture
     */
    void bindTexture();


    /**
     * @brief clear clears stencil/depth buffer
     */
    void clear();

    /**
     * @brief drawOnScreen draws FBO on screen
     * @param screen_shader is shader for screen drawing
     */
    void drawOnScreen(shader* screen_shader);

    /**
     * @brief unbindFBO unbinds FBO
     */
    void unbindFBO();
};

#endif // GLFBO_H
