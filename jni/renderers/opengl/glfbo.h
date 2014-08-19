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
    GLuint* rboID;                ///< Render buffer object id
    GLuint* fboID;                ///< Frame buffer object id
    texture* rtt;                 ///< Texture for color buffer loaded from file
    glvbo* vbo;                   ///< VBO for rendering to screen

    /**
     * @brief glfbo is an empty constructor
     */
    glfbo();

    /**
     * @brief glfbo creates framebuffer from raster data
     * @param texture is texture raster instance
     */
    glfbo(texture *texture);

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
     * @brief clear clears fragmet/depth buffer
     * @param colors true to clear both, false to clear only depth buffer
     */
    void clear(bool colors);

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
