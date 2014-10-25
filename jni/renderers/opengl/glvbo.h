//----------------------------------------------------------------------------------------
/**
 * \file       glvbo.h
 * \author     Vonasek Lubos
 * \date       2014/02/11
 * \brief      Vertex buffer object for OpenGL
*/
//----------------------------------------------------------------------------------------

#ifndef GLVBO_H
#define GLVBO_H

#include "interfaces/vbo.h"

class glvbo : public vbo
{
public:

    unsigned int instance;         ///< VBO for current object
    int size;

    /**
     * @brief removes all data from memory
     */
    ~glvbo();

    /**
     * @brief glvbo creates VBO from data
     * @param size is amount of vertices
     * @param vertices is vertices array
     * @param normals is normals array
     * @param coords is texture coords array
     * @param tnormals is triangle normals array
     */
    glvbo(int size, float* vertices, float* normals, float* coords, float* tnormals, bool dynamic);

    /**
     * @brief render renders vbo
     * @param sh is shader for rendering
     * @param begin is index of first vector/triangle
     * @param len is length of data to renderer
     */
    void render(shader* sh, int begin, int len);

    /**
     * @brief update updates VBO data
     * @param size is amount of vertices
     * @param vertices is vertices array
     * @param normals is normals array
     * @param coords is texture coords array
     * @param tnormals is triangle normals array
     */
    void update(int size, float* vertices, float* normals, float* coords, float* tnormals);
};

#endif // GLVBO_H
