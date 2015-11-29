///----------------------------------------------------------------------------------------
/**
 * \file       shader.h
 * \author     Vonasek Lubos
 * \date       2014/12/30
 * \brief      Class for compiling and operating on shader
**/
///----------------------------------------------------------------------------------------

#ifndef SHADER_H
#define SHADER_H

class shader
{
public:
    char shadername[256];  ///< Shader filename

    /**
     * @brief ~shader is destructor
     */
    virtual ~shader() {}

    /**
     * @brief it sets pointer to geometry
     * @param size is amount of data
     */
    virtual void attrib(unsigned int size) = 0;

    /**
     * @brief it sends geometry into GPU
     * @param vertices is vertices
     * @param normals is normals
     * @param coords is texture coords
     */
    virtual void attrib(float* vertices, float* normals, float* coords) = 0;

    /**
     * @brief it binds shader
     */
    virtual void bind() = 0;

    virtual bool hasAttrib(int i) = 0;

    /**
     * @brief it unbinds shader
     */
    virtual void unbind() = 0;

    /**
     * @brief uniformInt send int into shader
     * @param name is uniform name
     * @param value is uniform value
     */
    virtual void uniformInt(const char* name, int value) = 0;

    /**
     * @brief uniformFloat send float into shader
     * @param name is uniform name
     * @param value is uniform value
     */
    virtual void uniformFloat(const char* name, float value) = 0;

    /**
     * @brief uniformFloat4 send vec4 into shader
     * @param name is uniform name
     * @param a is vector x value
     * @param b is vector y value
     * @param c is vector z value
     * @param d is vector w value
     */
    virtual void uniformFloat4(const char* name, float a, float b, float c, float d) = 0;

    /**
     * @brief uniformMatrix send matrix into shader
     * @param name is uniform name
     * @param value is uniform value
     */
    virtual void uniformMatrix(const char* name, float* value) = 0;

};

#endif // SHADER_H
