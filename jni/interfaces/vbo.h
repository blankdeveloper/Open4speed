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

    /**
     * @brief destroy removes all data from memory
     */
    virtual void destroy() = 0;

    /**
     * @brief render renders vbo
     * @param sh is shader for rendering
     * @param begin is index of first vector/triangle
     * @param len is length of data to renderer
     * @param triangles is true when rendering triangles
     */
    virtual void render(shader* sh, int begin, int len, bool triangles) = 0;
};
#endif // VBO_H
