//----------------------------------------------------------------------------------------
/**
 * \file       glvbo.cpp
 * \author     Vonasek Lubos
 * \date       2014/02/11
 * \brief      Vertex buffer object for OpenGL
*/
//----------------------------------------------------------------------------------------

#include "renderers/opengl/gles20.h"
#include "renderers/opengl/glvbo.h"

/**
 * @brief glvbo creates VBO from data
 * @param size is amount of vertices
 * @param vertices is vertices array
 * @param normals is normals array
 * @param coords is texture coords array
 */
glvbo::glvbo(int size, float* vertices, float* normals, float* coords) {
    /// count buffer size
    this->size = size;
    int len = 0;
    if (vertices != 0)
        len += size * 3;
    if (normals != 0)
        len += size * 3;
    if (coords != 0)
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
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/**
 * @brief destroy removes all data from memory
 */
void glvbo::destroy() {
    glDeleteBuffers(1, &instance);
}

/**
 * @brief render renders vbo
 * @param sh is shader for rendering
 * @param begin is index of first vector/triangle
 * @param len is length of data to renderer
 */
void glvbo::render(shader* sh, int begin, int len) {
    glBindBuffer(GL_ARRAY_BUFFER, instance);
    sh->attrib(size);
    glDrawArrays(GL_TRIANGLES, begin * 3, len * 3);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
