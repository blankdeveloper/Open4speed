//----------------------------------------------------------------------------------------
/**
 * \file       glvbo.cpp
 * \author     Vonasek Lubos
 * \date       2014/02/11
 * \brief      Vertex buffer object for OpenGL
*/
//----------------------------------------------------------------------------------------


#include "stdafx.h"

/**
 * @brief glvbo creates VBO from data
 * @param size is amount of vertices
 * @param vertices is vertices array
 * @param normals is normals array
 * @param coords is texture coords array
 * @param tid is special array for lightmaps
 */
glvbo::glvbo(int size, float* vertices, float* normals, float* coords, float* tid) {
    /// count buffer size
    int len = 0;
    if (vertices != 0)
        len += size * 3;
    if (normals != 0)
        len += size * 3;
    if (coords != 0)
        len += size * 2;
    if (tid != 0)
        len += size * 2;

    glGenBuffers(1, &instance);
    glBindBuffer(GL_ARRAY_BUFFER, instance);
    glBufferData(GL_ARRAY_BUFFER, len, NULL, GL_STATIC_DRAW);
    len = 0;
    if (vertices != 0) {
        glBufferSubData(GL_ARRAY_BUFFER, len, size * 3, vertices);
        len += size * 3;
    }
    if (normals != 0) {
        glBufferSubData(GL_ARRAY_BUFFER, len, size * 3, normals);
        len += size * 3;
    }
    if (coords != 0) {
        glBufferSubData(GL_ARRAY_BUFFER, len, size * 2, coords);
        len += size * 2;
    }
    if (tid != 0) {
        glBufferSubData(GL_ARRAY_BUFFER, len, size * 2, tid);
        len += size * 2;
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/**
 * @brief bind binds VBO
 */
void glvbo::bind() {
    glBindBuffer(GL_ARRAY_BUFFER, instance);
}

/**
 * @brief destroy removes all data from memory
 */
void glvbo::destroy() {
    glDeleteBuffers(1, &instance);
}

/**
 * @brief unbind unbinds VBO
 */
void glvbo::unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
