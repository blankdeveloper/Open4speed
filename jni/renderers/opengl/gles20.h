///----------------------------------------------------------------------------------------
/**
 * \file       gles20.h
 * \author     Vonasek Lubos
 * \date       2014/12/30
 * \brief      GL renderer draws geometry and other things on screen
**/
///----------------------------------------------------------------------------------------


#ifndef GLES20_H
#define GLES20_H


#ifdef ANDROID
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#else
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif
#include <vector>
#include "interfaces/renderer.h"
#include "renderers/opengl/glsl.h"

/**
 * @brief The gles20 class is implementation of OpenGL ES 2.0
 */
class gles20 : public renderer
{
public:
    GLuint gpuMeasuring[1];               ///< Profiling
    shader* current;                      ///< Current binded shader
    glsl* scene;                          ///< Scene shader
    glsl* shadow;                         ///< Special shader for shadow
    bool oddFrame;                        ///< Odd frame info
    bool rttComplete;                     ///< Information if fbo is complete
    unsigned int* rendertexture;          ///< Texture for color buffer
    unsigned int* fboID;                  ///< Frame buffer object id
    unsigned int* rboID;                  ///< Render buffer object id

    /**
     * @brief gles20 constructor
     */
    gles20();

    /**
     * @brief gles20 destructor
     */
    ~gles20();

    /**
     * @brief init inits renderer
     * @param w is screen width
     * @param h is screen height
     * @param a is screen aliasing(reducing resolution)
     */
    void init(int w, int h, float a);

    /**
     * @brief renderDynamic render dynamic objects
     * @param geom is geometry vbo
     * @param sh is shader to use
     * @param txt is texture to use
     * @param triangleCount is triangle count
     */
    void renderDynamic(float* vertices, float* normals, float* coords, shader* sh, texture* txt, int triangleCount);

    /**
     * @brief renderModel renders model into scene
     * @param m is instance of model to render
     */
    void renderModel(model* m);

    /**
     * @brief renderShadow renders shadow of model into scene
     * @param m is instance of model to render
     */
    void renderShadow(model* m);

    /**
     * @brief renderSubModel renders model into scene
     * @param m is instance of model to render
     */
    void renderSubModel(model3d *m);

    /**
     * @brief rtt enables rendering into FBO which makes posible to do reflections
     * @param enable is true to start drawing, false to render on screen
     */
    void rtt(bool enable);

private:
    void cleanup();
};

#endif // GLES20_H
