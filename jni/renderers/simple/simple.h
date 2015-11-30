///----------------------------------------------------------------------------------------
/**
 * \file       simple.h
 * \author     Vonasek Lubos
 * \date       2015/11/29
 * \brief      Simple rendering 3D geometry
**/
///----------------------------------------------------------------------------------------

#ifndef SIMPLE_H
#define SIMPLE_H

#include <stack>
#include "interfaces/renderer.h"

// Struct for color of pixel r,g,b red, green, blue values
struct Color {
    unsigned char r,g,b;
};

enum DrawMode {
    Normal, Marker1, Marker2
};

/**
 * @brief The simple 2D renderer
 */
class simple : public renderer {
public:

    /**
     * @brief simple constructor
     */
    simple();

    /**
     * @brief simple is constructor
     * @param width is width of viewport
     * @param height is height of viewport
     */
    void init(int width, int height);

    /**
     * @brief renderer destructor
     */
    ~simple();

    /**
     * @brief clear clears depth and pixel buffers
     */
    void clear();

    /**
     * @brief getBuffer gets pixel buffer
     * @return pointer to pixel buffer
     */
    unsigned char* getBuffer() { return (unsigned char*)pixelBuffer; }

    /**
      * @brief line draws line into pixel buffer
      * @param x1 is line start position x
      * @param y1 is line start position y
      * @param x2 is line end position x
      * @param y2 is line end position y
      * @param dm is draw mode to be used
      * @return true if line was drawed
      */
    bool line(int x1, int y1, int x2, int y2, glm::dvec3 z1, glm::dvec3 z2, DrawMode dm);

    /**
     * @brief setColor sets RGB current color
     * @param r is red color
     * @param g is green color
     * @param b is blue color
     */
    void setColor(unsigned char r, unsigned char g, unsigned char b);

    void triangle(int x1, int y1, int x2, int y2, int x3, int y3, glm::dvec3 z1, glm::dvec3 z2, glm::dvec3 z3);

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
    void renderDynamic(vbo *geom, shader* sh, texture* txt, int triangleCount) { /*unsupported*/ }

    /**
     * @brief renderModel renders model into scene
     * @param m is instance of model to render
     */
    void renderModel(model* m);

    /**
     * @brief renderShadow renders shadow of model into scene
     * @param m is instance of model to render
     */
    void renderShadow(model* m) { /*unsupported*/ }

    /**
     * @brief renderSubModel renders model into scene
     * @param m is instance of model to render
     */
    void renderSubModel(model* mod, model3d *m);

    /**
     * @brief rtt enables rendering into FBO which makes posible to do reflections
     * @param enable is true to start drawing, false to render on screen
     */
    void rtt(bool enable);
private:

    /**
     * @brief test is Liang & Barsky clipping
     */
    bool test(double p, double q, double &t1, double &t2);

    void triangles(float* vertices, int offset, int size);

    // rendering
    int viewport_x, viewport_y, viewport_width, viewport_height;
    double* depthBuffer;
    Color* pixelBuffer;
    Color clearColor;
    Color currentColor;
    std::pair<int, glm::dvec3>* fillCache1;
    std::pair<int, glm::dvec3>* fillCache2;
    unsigned int timestamp;

    int xm, xp, ym, yp;                   ///< Current view frustum culling
    float camX;                           ///< Camera position x
    float camY;                           ///< Camera position y
    float camZ;                           ///< Camera position z
    float direction;                      ///< Camera direction
    glm::mat4x4 matrix;                   ///< Matrix for dynamic rendering
    glm::mat4x4 modelMat;                 ///< Model matrix
    glm::mat4x4 matrix_result;            ///< Temp matrix for calculations
    std::stack<glm::mat4x4> matrixBuffer; ///< Matrix stack
    float viewDistance;                   ///< Camera far culling

};

#endif // SIMPLE_H
