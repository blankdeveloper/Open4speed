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
#include <stack>
#include <vector>
#include "interfaces/renderer.h"
#include "renderers/opengl/glfbo.h"

/**
 * @brief The gles20 class is implementation of OpenGL ES 2.0
 */
class gles20 : public renderer {
public:
    int xm, xp, ym, yp;                   ///< Current view frustum culling
    float camX;                           ///< Camera position x
    float camY;                           ///< Camera position y
    float camZ;                           ///< Camera position z
    float direction;                      ///< Camera direction
    glm::mat4x4 matrix;                   ///< Matrix for dynamic rendering
    glm::mat4x4 modelMat;                 ///< Model matrix
    glm::mat4x4 matrix_result;            ///< Temp matrix for calculations
    std::stack<glm::mat4x4> matrixBuffer; ///< Matrix stack
    GLushort dynindices[4095];            ///< Indicies for dynamic rendering
    shader* current;                      ///< Current binded shader
    shader* shadow;                       ///< Special shader for shadow
    std::vector<glfbo*> rtt_fbo;          ///< Screen-space framebuffer
    bool oddFrame;                        ///< Odd frame info
    float viewDistance;                   ///< Camera far culling

    /**
     * @brief gles20 destructor
     */
    ~gles20();

    /**
     * @brief gles20 constructor
     */
    gles20();

    void init(int w, int h);

    /**
     * @brief lookAt implements GLUlookAt
     * @param eye is eye vector
     * @param center is camera center
     * @param up is up vector
     */
    void lookAt(glm::vec3 eye, glm::vec3 center, glm::vec3 up);

    /**
     * @brief perspective implements GLUPerspective
     * @param fovy is fov angle
     * @param aspect is screen aspect ration
     * @param zNear is near cutting plate
     * @param zFar is far cutting plane
     */
    void perspective(float fovy, float aspect, float zNear, float zFar);

    /**
     * @brief multMatrix multiplies with matrix
     * @param matrix is 4x4 matrix in OpenGL format
     */
    void multMatrix(float* matrix);

    /**
     * @brief popMatrix pops matrix from stack
     */
    void popMatrix();

    /**
     * @brief pushMatrix pushs current matrix to stack
     */
    void pushMatrix();

    /**
     * @brief rotateX rotate around X axis
     * @param value is angle
     */
    void rotateX(float value);

    /**
     * @brief rotateX rotate around Y axis
     * @param value is angle
     */
    void rotateY(float value);

    /**
     * @brief rotateX rotate around Z axis
     * @param value is angle
     */
    void rotateZ(float value);

    /**
     * @brief scale scales current matrix
     * @param value is amount of scale(1 to keep current)
     */
    void scale(float value);

    /**
     * @brief translate translates object
     * @param x is translate coordinate
     * @param y is translate coordinate
     * @param z is translate coordinate
     */
    void translate(float x, float y, float z);

    /**
     * @brief renderDynamic render dynamic objects
     * @param geom is geometry vbo
     * @param sh is shader to use
     * @param txt is texture to use
     * @param triangleCount is triangle count
     */
    void renderDynamic(vbo *geom, shader* sh, texture* txt, int triangleCount);

    /**
     * @brief renderModel renders model into scene
     * @param m is instance of model to render
     * @param center is model translation
     */
    void renderModel(model* m, glm::vec3 center);

    /**
     * @brief renderShadow renders shadow of model into scene
     * @param m is instance of model to render
     * @param center is model translation
     */
    void renderShadow(model* m, glm::vec3 center);

    /**
     * @brief renderSubModel renders model into scene
     * @param m is instance of model to render
     * @param center is model translation
     */
    void renderSubModel(model* mod, model3d *m, glm::vec3 center);

    /**
     * @brief rtt enables rendering into FBO which makes posible to do reflections
     * @param enable is true to start drawing, false to render on screen
     */
    void rtt(bool enable);
};

#endif // GLES20_H
