//----------------------------------------------------------------------------------------
/**
 * \file       glfbo.h
 * \author     Vonasek Lubos
 * \date       2014/02/10
 * \brief      Framebuffer object class for OpenGL
*/
//----------------------------------------------------------------------------------------


#ifndef GLFBO_H
#define GLFBO_H

#include "renderers/opengl/gles20.h"
#include "renderers/opengl/glvbo.h"

class glfbo : public fbo
{
public:
    GLuint* rendertexture;        ///< Texture for color buffer
    GLuint* fboID;                ///< Frame buffer object id
    GLuint* rboID;                ///< Render buffer object id
    glvbo* rect;                  ///< VBO for rendering to screen

    /**
     * @brief glfbo is an empty constructor
     */
    glfbo();

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
     * @brief destroy removes all data from memory
     */
    void destroy();

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
