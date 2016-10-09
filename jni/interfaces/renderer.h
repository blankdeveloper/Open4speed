///----------------------------------------------------------------------------------------
/**
 * \file       renderer.h
 * \author     Vonasek Lubos
 * \date       2014/12/30
 * \brief      Renderer draws geometry and other things on screen
**/
///----------------------------------------------------------------------------------------

#ifndef RENDERER_H
#define RENDERER_H

#include <glm/glm.hpp>
#include "engine/model.h"
#include "interfaces/fbo.h"
#include "interfaces/shader.h"
#include "interfaces/texture.h"

const int culling = 150;              ///< View culling distance in meters

/**
 * @brief The renderer interface
 */
class renderer {
public:

    float aliasing;             ///< Screen detail
    bool enable[10];            ///< Enabled filter
    glm::mat4x4 proj_matrix;    ///< Scene projection matrix
    glm::mat4x4 view_matrix;    ///< View matrix
    shader* scene_shader;       ///< Scene shader
    int screen_width;           ///< Screen width
    int screen_height;          ///< Screen height

    virtual ~renderer() {}

    virtual void init(int w, int h) = 0;

    /**
     * @brief lookAt implements GLUlookAt
     * @param eye is eye vector
     * @param center is camera center
     * @param up is up vector
     */
    virtual void lookAt(glm::vec3 eye, glm::vec3 center, glm::vec3 up) = 0;

    /**
     * @brief perspective implements GLUPerspective
     * @param fovy is fov angle
     * @param aspect is screen aspect ration
     * @param zNear is near cutting plate
     * @param zFar is far cutting plane
     */
    virtual void perspective(float fovy, float aspect, float zNear, float zFar) = 0;

    /**
     * @brief multMatrix multiplies with matrix
     * @param matrix is 4x4 matrix in OpenGL format
     */
    virtual void multMatrix(float* matrix) = 0;

    /**
     * @brief popMatrix pops matrix from stack
     */
    virtual void popMatrix() = 0;

    /**
     * @brief pushMatrix pushs current matrix to stack
     */
    virtual void pushMatrix() = 0;

    /**
     * @brief rotateX rotate around X axis
     * @param value is angle
     */
    virtual void rotateX(float value) = 0;

    /**
     * @brief rotateX rotate around Y axis
     * @param value is angle
     */
    virtual void rotateY(float value) = 0;

    /**
     * @brief rotateX rotate around Z axis
     * @param value is angle
     */
    virtual void rotateZ(float value) = 0;

    /**
     * @brief scale scales current matrix
     * @param value is amount of scale(1 to keep current)
     */
    virtual void scale(float value) = 0;

    /**
     * @brief translate translates object
     * @param x is translate coordinate
     * @param y is translate coordinate
     * @param z is translate coordinate
     */
    virtual void translate(float x, float y, float z) = 0;

    /**
     * @brief renderDynamic render dynamic objects
     * @param geom is geometry vbo
     * @param sh is shader to use
     * @param txt is texture to use
     * @param triangleCount is triangle count
     */
    virtual void renderDynamic(float* vertices, float* normals, float* coords, shader* sh, texture* txt, int triangleCount) = 0;

    /**
     * @brief renderModel renders model into scene
     * @param m is instance of model to render
     */
    virtual void renderModel(model* m) = 0;

    /**
     * @brief renderShadow renders shadow of model into scene
     * @param m is instance of model to render
     */
    virtual void renderShadow(model* m) = 0;

    /**
     * @brief renderSubModel renders model into scene
     * @param m is instance of model to render
     */
    virtual void renderSubModel(model3d *m) = 0;

    /**
     * @brief rtt enables rendering into FBO which makes posible to do reflections
     * @param enable is true to start drawing, false to render on screen
     */
    virtual void rtt(bool enable) = 0;
};

#endif // RENDERER_H
