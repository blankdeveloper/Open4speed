///----------------------------------------------------------------------------------------
/**
 * \file       glsl.h
 * \author     Vonasek Lubos
 * \date       2014/12/30
 * \brief      Class for compiling and operating on GLSL shader
**/
///----------------------------------------------------------------------------------------

#ifndef GLSL_H
#define GLSL_H

#include <string>
#include "interfaces/shader.h"

class glsl : public shader
{
public:
    unsigned int id;          ///< Shader id
    unsigned int shader_vp;   ///< Vertex shader
    unsigned int shader_fp;   ///< Fragment shader
    int attribute_v_vertex;   ///< VBO vertices
    int attribute_v_coord;    ///< VBO coords
    int attribute_v_normal;   ///< VBO normals

    ~glsl();

    /**
     * @brief Constructor
     * @param vert is vertex shader code
     * @param frag is fragment shader code
     */
    glsl(std::vector<std::string> vert, std::vector<std::string> frag);

    /**
     * @brief it sets pointer to geometry
     * @param size is amount of data
     */
    void attrib(unsigned int size);

    /**
     * @brief it sends geometry into GPU
     * @param vertices is vertices
     * @param normals is normals
     * @param coords is texture coords
     */
    void attrib(float* vertices, float* normals, float* coords);

    /**
     * @brief it binds shader
     */
    void bind();

    /**
     * @brief initShader creates shader from code
     * @param vs is vertex shader code
     * @param fs is fragment shader code
     * @return shader program id
     */
    unsigned int initShader(const char *vs, const char *fs);

    /**
     * @brief it unbinds shader
     */
    void unbind();

    /**
     * @brief uniformInt send int into shader
     * @param name is uniform name
     * @param value is uniform value
     */
    void uniformInt(const char* name, int value);

    /**
     * @brief uniformFloat send float into shader
     * @param name is uniform name
     * @param value is uniform value
     */
    void uniformFloat(const char* name, float value);

    /**
     * @brief uniformFloat4 send vec4 into shader
     * @param name is uniform name
     * @param a is vector x value
     * @param b is vector y value
     * @param c is vector z value
     * @param d is vector w value
     */
    void uniformFloat4(const char* name, float a, float b, float c, float d);

    /**
     * @brief uniformMatrix send matrix into shader
     * @param name is uniform name
     * @param value is uniform value
     */
    void uniformMatrix(const char* name, float* value);
};

#endif // GLSL_H
