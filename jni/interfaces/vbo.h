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
     * @brief bind binds VBO
     */
    virtual void bind() = 0;

    /**
     * @brief destroy removes all data from memory
     */
    virtual void destroy() = 0;

    /**
     * @brief render renders vbo
     * @param sh is shader for rendering
     * @param begin is index of first vector/triangle
     * @param len is length of data to renderer
     * @param size is length of full VBO
     * @param triangles is true when rendering triangles
     */
    virtual void render(shader* sh, int begin, int len, int size, bool triangles) = 0;

    /**
     * @brief unbind unbinds VBO
     */
    virtual void unbind() = 0;

};
#endif // VBO_H
