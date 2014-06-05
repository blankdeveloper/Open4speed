//----------------------------------------------------------------------------------------
/**
 * \file       glfbo.cpp
 * \author     Vonasek Lubos
 * \date       2014/02/10
 * \brief      Framebuffer object class for OpenGL
*/
//----------------------------------------------------------------------------------------

#ifdef ANDROID
#include <jni.h>
#endif
#include <cstdlib>
#include "renderers/opengl/glfbo.h"
#include "utils/math.h"
#include "common.h"

/**
 * @brief glfbo is an empty constructor
 */
glfbo::glfbo() {

}


/**
 * @brief glfbo creates framebuffer from raster data
 * @param texture is texture raster instance
 */
glfbo::glfbo(texture *texture) {

        //set resolution
        width = texture->twidth;
        height = texture->twidth;

        //create frame buffer
        fboID = new GLuint[1];
        rboID = new GLuint[1];
        rendertexture = new GLuint[1];
        rendertexture2 = new GLuint[1];
        depth = false;
        rb = false;

        rtt = texture;
        rendertexture[0] = rtt->textureID;
        glGenFramebuffers(1, fboID);
        glBindFramebuffer(GL_FRAMEBUFFER, fboID[0]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rendertexture[0], 0);

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
    this->width = width;
    this->height = height;

    //create frame buffer
    fboID = new GLuint[1];
    rboID = new GLuint[1];
    rendertexture = new GLuint[1];
    rendertexture2 = new GLuint[1];

    glGenFramebuffers(1, fboID);
    glBindFramebuffer(GL_FRAMEBUFFER, fboID[0]);
    glGenTextures(1, rendertexture);
    glBindTexture(GL_TEXTURE_2D, rendertexture[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rendertexture[0], 0);

    //create texture for depth buffer
    this->depth = depthbuffer;
    rb = !depthbuffer;
    if (depthbuffer) {
        glGenTextures(1, rendertexture2);
        glBindTexture(GL_TEXTURE_2D, rendertexture2[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, rendertexture2[0], 0);
    }
    //create render buffer
    else {
        glGenRenderbuffers(1, rboID);
        glBindRenderbuffer(GL_RENDERBUFFER, rboID[0]);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboID[0]);
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
    glBindFramebuffer(GL_FRAMEBUFFER, fboID[0]);
    glViewport (0, 0, width, height);
}

/**
 * @brief bind binds texture
 */
void glfbo::bindTexture() {
    glBindTexture(GL_TEXTURE_2D, rendertexture[0]);
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
 * @brief destroy removes all data from memory
 */
void glfbo::destroy() {
    if (!depth && !rb) {
        rtt->pointerDecrease();
    }
    if (depth) {
        glDeleteTextures(1, rendertexture2);
    }
    if (rb) {
        glDeleteRenderbuffers(1, rboID);
    }
    glDeleteFramebuffers(1, fboID);
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
                        1, 0,
                        1, 1,
                        0, 1
    };

    /// texture
    glBindTexture(GL_TEXTURE_2D, rendertexture[0]);
    screen_shader->attrib(vertices, coords);

    /// render
    glDisable(GL_DEPTH_TEST);
    glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLubyte), GL_UNSIGNED_BYTE, indices);
    screen_shader->unbind();
}

/**
 * @brief unbindFBO unbinds FBO
 */
void glfbo::unbindFBO() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport (0, 0, screen_width, screen_height);
}
