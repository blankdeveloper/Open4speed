//----------------------------------------------------------------------------------------
/**
 * \file       glsl.cpp
 * \author     Vonasek Lubos
 * \date       2014/01/05
 * \brief      Class for compiling and operating on GLSL shader
*/
//----------------------------------------------------------------------------------------

#include "renderers/opengl/glsl.h"
#include "utils/io.h"

#ifdef ANDROID
const char* header = "#version 100\nprecision mediump float;\n";    ///< Shader header
#else
const char* header = "#version 130\nprecision highp float;\n";      ///< Shader header
#endif

/**
 * @brief initShader creates shader from code
 * @param vs is vertex shader code
 * @param fs is fragment shader code
 * @return shader program id
 */
unsigned int initShader(const char *vs, const char *fs) {

    /// Load shader
    GLuint shader_vp = glCreateShader(GL_VERTEX_SHADER);
    GLuint shader_fp = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(shader_vp, 1, &vs, 0);
    glShaderSource(shader_fp, 1, &fs, 0);

    /// Alocate buffer for logs
    const unsigned int BUFFER_SIZE = 512;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    GLsizei length = 0;

    /// Compile shaders
    glCompileShader(shader_vp);
    glGetShaderInfoLog(shader_vp, BUFFER_SIZE, &length, buffer);
    if (length > 0) {
        logi("GLSL compile log:", buffer);
    }
    glCompileShader(shader_fp);
    glGetShaderInfoLog(shader_fp, BUFFER_SIZE, &length, buffer);
    if (length > 0) {
        logi("GLSL compile log:", buffer);
    }

    /// Link program
    unsigned int shader_id = glCreateProgram();
    glAttachShader(shader_id, shader_fp);
    glAttachShader(shader_id, shader_vp);
    glLinkProgram(shader_id);
    glGetProgramInfoLog(shader_id, BUFFER_SIZE, &length, buffer);
    if (length > 0) {
        logi("GLSL program info log:", buffer);
    }

    /// Check shader
    glValidateProgram(shader_id);
    GLint status;
    glGetProgramiv(shader_id, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        logi("GLSL error linking", "buffer");
    }
    return shader_id;
}

/**
 * @brief Constructor
 * @param vert is vertex shader code
 * @param frag is fragment shader code
 */
glsl::glsl(std::vector<char*> *vert, std::vector<char*> *frag) {
    /// convert vertex shader source code
    int size = strlen(header);
    for (unsigned int i = 0; i < vert->size(); i++) {
        size += strlen((*vert)[i]) + 2;
    }
    char* vs = new char[size];
    strcpy(vs, header);
    for (unsigned int i = 0; i < vert->size(); i++) {
        strcat(vs, (*vert)[i]);
        strcat(vs, "\n");
    }

    /// convert fragment shader source code
    size = strlen(header);
    for (unsigned int i = 0; i < frag->size(); i++) {
        size += strlen((*frag)[i]) + 2;
    }
    char* fs = new char[size];
    strcpy(fs, header);
    for (unsigned int i = 0; i < frag->size(); i++) {
        strcat(fs, (*frag)[i]);
        strcat(fs, "\n");
    }

    /// compile shader
    id = initShader(vs, fs);

    /// Attach VBO attributes
    attribute_v_vertex = glGetAttribLocation(id, "v_vertex");
    attribute_v_coord = glGetAttribLocation(id, "v_coord");
    attribute_v_normal = glGetAttribLocation(id, "v_normal");
    attribute_v_tid = glGetAttribLocation(id, "v_tid");
}

/**
 * @brief it sets pointer to geometry
 * @param size is amount of data
 */
void glsl::attrib(int size) {

    /// apply attributes
    glVertexAttribPointer(attribute_v_vertex, 3, GL_FLOAT, GL_FALSE, 0, ( const GLvoid *) 0);
    if (attribute_v_normal != -1)
        glVertexAttribPointer(attribute_v_normal, 3, GL_FLOAT, GL_FALSE, 0, ( const GLvoid *) (size * 3));
    if (attribute_v_coord != -1)
        glVertexAttribPointer(attribute_v_coord, 2, GL_FLOAT, GL_FALSE, 0, ( const GLvoid *) (size * 6));
    if (attribute_v_tid != -1)
        glVertexAttribPointer(attribute_v_tid, 2, GL_FLOAT, GL_FALSE, 0, ( const GLvoid *) (size * 8));
}

/**
 * @brief it sends geometry into GPU
 * @param vertices is vertices
 * @param coords is texture coords
 */
void glsl::attrib(float* vertices, float* coords) {
    /// send attributes to GPU
    glVertexAttribPointer(attribute_v_vertex, 3, GL_FLOAT, GL_FALSE, 0, vertices);
    if (attribute_v_normal != -1)
        glVertexAttribPointer(attribute_v_normal, 3, GL_FLOAT, GL_FALSE, 0, vertices);
    glVertexAttribPointer(attribute_v_coord, 2, GL_FLOAT, GL_FALSE, 0, coords);
}

/**
 * @brief it binds shader
 */
void glsl::bind() {
    /// bind shader
    glUseProgram(id);

    /// set attributes
    glEnableVertexAttribArray(attribute_v_vertex);
    if (attribute_v_coord != -1)
        glEnableVertexAttribArray(attribute_v_coord);
    if (attribute_v_normal != -1)
        glEnableVertexAttribArray(attribute_v_normal);
    if (attribute_v_tid != -1)
        glEnableVertexAttribArray(attribute_v_tid);
}

/**
 * @brief it unbinds shader
 */
void glsl::unbind() {
    glUseProgram(0);
}

/**
 * @brief uniformInt send int into shader
 * @param name is uniform name
 * @param value is uniform value
 */
void glsl::uniformInt(const char* name, int value) {
    glUniform1i(glGetUniformLocation(id, name), value);
}

/**
 * @brief uniformFloat send float into shader
 * @param name is uniform name
 * @param value is uniform value
 */
void glsl::uniformFloat(const char* name, float value) {
    glUniform1f(glGetUniformLocation(id, name), value);
}

/**
 * @brief uniformFloat4 send vec4 into shader
 * @param name is uniform name
 * @param a is vector x value
 * @param b is vector y value
 * @param c is vector z value
 * @param d is vector w value
 */
void glsl::uniformFloat4(const char* name, float a, float b, float c, float d) {
    glUniform4f(glGetUniformLocation(id, name), a, b, c, d);
}

/**
 * @brief uniformMatrix send matrix into shader
 * @param name is uniform name
 * @param value is uniform value
 */
void glsl::uniformMatrix(const char* name, float* value) {
    glUniformMatrix4fv(glGetUniformLocation(id,name),1, GL_FALSE, value);
}
