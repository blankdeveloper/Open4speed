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
     * @brief unbind unbinds VBO
     */
    virtual void unbind() = 0;

};
#endif // VBO_H
