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
     * @brief render renders vbo
     * @param sh is shader for rendering
     * @param begin is index of first vector/triangle
     * @param len is length of data to renderer
     */
    virtual void render(shader* sh, int begin, int len) = 0;
};
#endif // VBO_H
