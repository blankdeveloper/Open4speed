//----------------------------------------------------------------------------------------
/**
 * \file       vbo.h
 * \author     Vonasek Lubos
 * \date       2014/02/11
 * \brief      Vertex buffer object interface
*/
//----------------------------------------------------------------------------------------

#ifndef VBO_H
#define VBO_H

#include "interfaces/shader.h"

class vbo
{
public:

    virtual ~vbo() {}

    /**
     * @brief render renders vbo
     * @param sh is shader for rendering
     * @param begin is index of first vector/triangle
     * @param len is length of data to renderer
     */
    virtual void render(shader* sh, int begin, int len) = 0;

    /**
     * @brief update updates VBO data
     * @param size is amount of vertices
     * @param vertices is vertices array
     * @param normals is normals array
     * @param coords is texture coords array
     * @param tnormals is triangle normals array
     */
    virtual void update(int size, float* vertices, float* normals, float* coords, float* tnormals) = 0;
};
#endif // VBO_H
