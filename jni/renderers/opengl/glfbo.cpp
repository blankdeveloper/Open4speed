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
#include <string.h>
#include "renderers/opengl/gles20.h"
#include "renderers/opengl/glfbo.h"
#include "utils/io.h"

#ifdef ANDROID
#define PACKED_EXTENSION "GL_OES_packed_depth_stencil"
#define PACKED_EXT GL_DEPTH24_STENCIL8_OES
#else
#define PACKED_EXTENSION "GL_EXT_packed_depth_stencil"
#define PACKED_EXT GL_DEPTH24_STENCIL8_EXT
#endif

/**
 * @brief removes all data from memory
 */
glfbo::~glfbo() {
    glDeleteRenderbuffers(complete ? 2 : 1, rboID);
    glDeleteFramebuffers(1, fboID);
    delete rect;
    delete[] fboID;
    delete[] rboID;
    delete[] rendertexture;
}

/**
 * @brief constructor init framebuffer and renderbuffer
 * @param width is width of framebuffer
 * @param height is height of framebuffer
 * @param depthbuffer is true to use depthbuffer texture
 */
glfbo::glfbo(int width, int height) {

    //find ideal texture resolution
    this->width = width;
    this->height = height;

    //create frame buffer
    fboID = new GLuint[1];
    rboID = new GLuint[2];
    rendertexture = new GLuint[1];

    //framebuffer texture
    glGenFramebuffers(1, fboID);
    glBindFramebuffer(GL_FRAMEBUFFER, fboID[0]);
    glGenTextures(1, rendertexture);
    glBindTexture(GL_TEXTURE_2D, rendertexture[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width * aliasing, height * aliasing, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rendertexture[0], 0);


    /// create render buffers for depth buffer and stencil buffer
    glGenRenderbuffers(2, rboID);
    glBindRenderbuffer(GL_RENDERBUFFER, rboID[0]);
    char* extString = (char*)glGetString(GL_EXTENSIONS);
    if (strstr(extString, PACKED_EXTENSION) != 0)
    {
        glRenderbufferStorage(GL_RENDERBUFFER, PACKED_EXT, width * aliasing, height * aliasing);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboID[0]);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboID[0]);
    }
    else
    {
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width * aliasing, height * aliasing);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboID[0]);
        glBindRenderbuffer(GL_RENDERBUFFER, rboID[1]);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, width * aliasing, height * aliasing);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboID[1]);
    }

    /// print supported extensions into log
    for (unsigned int i = 0; i < strlen(extString); i++)
        if (extString[i] == ' ')
            extString[i] = '\n';
    logi("Extensions:\n", extString);

    /// check FBO status
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
        complete = true;
    else
    {
        char error[128];
        sprintf(error, "%d", glCheckFramebufferStatus(GL_FRAMEBUFFER));
        logi("glCheckFramebufferStatus", error);
        complete = false;
    }

    /// clear
    glViewport (0, 0, width * aliasing, height * aliasing);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /// vertices
    float vertices[] = {
        -1, +1, 0,
        -1, -1, 0,
        +1, -1, 0,
        -1, +1, 0,
        +1, -1, 0,
        +1, +1, 0,
    };

    /// coords
    float coords[] = {
        0, 1,
        0, 0,
        1, 0,
        0, 1,
        1, 0,
        1, 1,
    };

    /// create vertex buffer
    rect = new glvbo(6, vertices, 0, coords, 0, false);
}

/**
 * @brief bind binds FBO
 */
void glfbo::bindFBO() {
    glBindFramebuffer(GL_FRAMEBUFFER, fboID[0]);
    glViewport (0, 0, width * aliasing, height * aliasing);
}

/**
 * @brief bind binds texture
 */
void glfbo::bindTexture() {
    glBindTexture(GL_TEXTURE_2D, rendertexture[0]);
}

/**
 * @brief clear clears depth buffer
 */
void glfbo::clear() {
    glClear(GL_DEPTH_BUFFER_BIT);
}

/**
 * @brief drawOnScreen draws FBO on screen
 * @param screen_shader is shader for screen drawing
 */
void glfbo::drawOnScreen(shader* screen_shader) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport (0, 0, width, height);
    screen_shader->bind();
    glBindTexture(GL_TEXTURE_2D, rendertexture[0]);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(false);
    rect->render(screen_shader, 0, 2);
}

/**
 * @brief unbindFBO unbinds FBO
 */
void glfbo::unbindFBO() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
