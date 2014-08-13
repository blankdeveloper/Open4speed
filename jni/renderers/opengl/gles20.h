//----------------------------------------------------------------------------------------
/**
 * \file       gles20.h
 * \author     Vonasek Lubos
 * \date       2014/02/23
 * \brief      GL renderer draws geometry and other things on screen
*/
//----------------------------------------------------------------------------------------


#ifndef GLES20_H
#define GLES20_H


#ifdef ANDROID
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#else
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#endif
#include <stack>
#include <vector>
#include "interfaces/renderer.h"
#include "interfaces/shader.h"


const int culling = 150;              ///< View culling distance in meters
const glm::mat4x4 eye = glm::mat4x4(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);

/**
 * @brief The gles20 class is implementation of OpenGL ES 2.0
 */
class gles20 : public renderer {
public:
    int xm, xp, ym, yp;                   ///< Current view frustum culling
    float camX;                           ///< Camera position x
    float camY;                           ///< Camera position y
    float camZ;                           ///< Camera position z
    glm::mat4x4 matrix;                   ///< Matrix for dynamic rendering
    glm::mat4x4 modelMat;                 ///< Model matrix
    glm::mat4x4 matrix_result;            ///< Temp matrix for calculations
    std::stack<glm::mat4x4> matrixBuffer; ///< Matrix stack
    GLushort dynindices[4095];            ///< Indicies for dynamic rendering
    shader* current;                      ///< Current binded shader
    shader* shadow;                       ///< Special shader for shadow

    /**
     * @brief gles20 constructor
     */
    gles20();

    /**
     * @brief lookAt implements GLUlookAt
     * @param eyex is eye vector coordinate
     * @param eyey is eye vector coordinate
     * @param eyez is eye vector coordinate
     * @param centerx is camera center coordinate
     * @param centery is camera center coordinate
     * @param centerz is camera center coordinate
     * @param upx is up vector coordinate
     * @param upy is up vector coordinate
     * @param upz is up vector coordinate
     */
    void lookAt(GLfloat eyex, GLfloat eyey, GLfloat eyez,
                GLfloat centerx, GLfloat centery, GLfloat centerz,
                GLfloat upx, GLfloat upy, GLfloat upz);

    /**
     * @brief perspective implements GLUPerspective
     * @param fovy is fov angle
     * @param aspect is screen aspect ration
     * @param zNear is near cutting plate
     * @param zFar is far cutting plane
     */
    void perspective(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar);

    /**
     * @brief clear clears fragmet/depth buffer
     * @param colors true to clear both, false to clear only depth buffer
     */
    void clear(bool colors);

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
     * @param vertices is vertices
     * @param coords is texture coords
     * @param sh is shader to use
     * @param txt is texture to use
     * @param triangleCount is triangle count
     */
    void renderDynamic(GLfloat *vertices, GLfloat *coords, shader* sh, texture* txt, int triangleCount);

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
    void renderSubModel(model* mod, model3d *m);
};

#endif // GLES20_H
