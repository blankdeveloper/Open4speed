///----------------------------------------------------------------------------------------
/**
 * \file       matrices.h
 * \author     Vonasek Lubos
 * \date       2016/10/12
 * \brief      Util for working with matrices
**/
///----------------------------------------------------------------------------------------

#ifndef MATRICES_H
#define MATRICES_H

#include <glm/glm.hpp>
#include <stack>

/**
 * @brief The input interface
 */
class matrices
{
public:
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

protected:
    glm::vec3 camera;                     ///< Camera position
    glm::vec3 direction;                  ///< Camera direction
    glm::mat4x4 matrix;                   ///< Matrix for dynamic rendering
    glm::mat4x4 modelMat;                 ///< Model matrix
    glm::mat4x4 matrix_result;            ///< Temp matrix for calculations
    glm::mat4x4 proj_matrix;              ///< Scene projection matrix
    glm::mat4x4 view_matrix;              ///< View matrix
    std::stack<glm::mat4x4> matrixBuffer; ///< Matrix stack
};

#endif // MATRICES_H
