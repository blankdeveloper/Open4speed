//----------------------------------------------------------------------------------------
/**
 * \file       glfbo.cpp
 * \author     Vonasek Lubos
 * \date       2014/02/10
 * \brief      Framebuffer object class for OpenGL
*/
//----------------------------------------------------------------------------------------

#include "stdafx.h"

/**
 * @brief glfbo is an empty constructor
 */
glfbo::glfbo() {

}


/**
 * @brief glfbo creates framebuffer from raster data
 * @param texture is texture raster instance
 */
glfbo::glfbo(Texture texture) {

        //find ideal texture resolution
        res = 2;
        res = max(texture.width, texture.height);
        width = texture.width;
        height = texture.height;

        //create frame buffer
        glGenFramebuffers(1, &fboID);
        glBindFramebuffer(GL_FRAMEBUFFER, fboID);
        glGenTextures(1, &rendertexture);
        glBindTexture(GL_TEXTURE_2D, rendertexture);
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        if (texture.hasAlpha) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width, texture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.data);
        } else {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.width, texture.height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture.data);
        }
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rendertexture, 0);

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            exit(1);

        //clear
        glViewport (0, 0, width, height);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


/**
 * @brief constructor init framebuffer and renderbuffer
 * @param width is width of framebuffer
 * @param height is height of framebuffer
 * @param depthbuffer is true to use depthbuffer texture
 */
glfbo::glfbo(int width, int height, bool depthbuffer) {

    //find ideal texture resolution
    res = 2;
    this->width = width;
    this->height = height;
    while (res < max(width, height)) {
        res *= 2;
    }

    //create frame buffer
    glGenFramebuffers(1, &fboID);
    glBindFramebuffer(GL_FRAMEBUFFER, fboID);
    glGenTextures(1, &rendertexture);
    glBindTexture(GL_TEXTURE_2D, rendertexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, res, res, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rendertexture, 0);

    //create texture for depth buffer
    if (depthbuffer) {
        glGenTextures(1, &rendertexture2);
        glBindTexture(GL_TEXTURE_2D, rendertexture2);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, res, res, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, rendertexture2, 0);
    }
    //create render buffer
    else {
        glGenRenderbuffers(1, &rboID);
        glBindRenderbuffer(GL_RENDERBUFFER, rboID);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, res, res);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboID);
    }


    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        exit(1);

    //clear
    glViewport (0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * @brief bind binds FBO
 */
void glfbo::bindFBO() {
    glBindFramebuffer(GL_FRAMEBUFFER, fboID);
    glViewport (0, 0, width, height);
}

/**
 * @brief bind binds texture
 */
void glfbo::bindTexture() {
    glBindTexture(GL_TEXTURE_2D, rendertexture);
}

/**
 * @brief clear clears fragmet/depth buffer
 * @param colors true to clear both, false to clear only depth buffer
 */
void glfbo::clear(bool colors) {
    if (colors) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    } else {
        glClear(GL_DEPTH_BUFFER_BIT);
    }
}

/**
 * @brief drawOnScreen draws FBO on screen
 * @param screen_shader is shader for screen drawing
 */
void glfbo::drawOnScreen(shader* screen_shader) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport (0, 0, (GLsizei) screen_width, (GLsizei) screen_height);
    screen_shader->bind();

    /// indicies
    GLubyte indices[] = {3,0,1,3,1,2};

    /// vertices
    GLfloat vertices[] = {
        -1, -1, 0,
        +1, -1, 0,
        +1, +1, 0,
        -1, +1, 0
    };

    /// coords
    GLfloat coords[] = {0, 0,
                        width / (float)res, 0,
                        width / (float)res, height / (float)res,
                        0, height / (float)res
    };

    /// texture
    glBindTexture(GL_TEXTURE_2D, rendertexture);
    screen_shader->attrib(vertices, coords);
    screen_shader->uniformFloat("u_res", 1 / (float)res);

    /// render
    glDisable(GL_DEPTH_TEST);
    glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLubyte), GL_UNSIGNED_BYTE, indices);
    screen_shader->unbind();
}
