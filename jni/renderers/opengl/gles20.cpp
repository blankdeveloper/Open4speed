///----------------------------------------------------------------------------------------
/**
 * \file       gles20.cpp
 * \author     Vonasek Lubos
 * \date       2014/12/30
 * \brief      GL renderer draws geometry and other things on screen
**/
///----------------------------------------------------------------------------------------

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "renderers/opengl/gles20.h"
#include "renderers/opengl/glfbo.h"
#include "renderers/opengl/gltexture.h"
#include "engine/switch.h"

/**
 * @brief gles20 destructor
 */
gles20::~gles20() {
    while (!rtt_fbo.empty()) {
        delete rtt_fbo[rtt_fbo.size() - 1];
        rtt_fbo.pop_back();
    }
}

/**
 * @brief gles20 constructor
 */
gles20::gles20() {

    /// set default values
    for (int i = 0; i < 10; i++) {
        enable[i] = true;
    }
    for (int i = 0; i < 4095; i++) {
        dynindices[i] = i;
    }
    aliasing = 1;
    oddFrame = true;
}

void gles20::init(int w, int h) {

    screen_width = w;
    screen_height = h;

    //find ideal texture resolution
    int resolution = 2;
    while (resolution < screen_width) {
        resolution *= 2;
    }

    //create render texture
    while (!rtt_fbo.empty()) {
        delete rtt_fbo[rtt_fbo.size() - 1];
        rtt_fbo.pop_back();
    }
    for (int i = 0; i < 2; i++) {
        rtt_fbo.push_back(new glfbo(screen_width, screen_height));
    }

    //set viewport
    glViewport (0, 0, (GLsizei) screen_width, (GLsizei) screen_height);
    glClear(GL_COLOR_BUFFER_BIT);
    glActiveTexture( GL_TEXTURE0 );

    //set shaders
    scene_shader = getShader("scene");
    shadow = getShader("shadow");
}

/**
 * @brief lookAt implements GLUlookAt
 * @param eye is eye vector
 * @param center is camera center
 * @param up is up vector
 */
void gles20::lookAt(glm::vec3 eye, glm::vec3 center, glm::vec3 up) {

    camera = eye;
    direction = glm::normalize(center - eye);
    view_matrix = glm::lookAt(eye, center, up);
    matrix_result = glm::mat4x4(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);
}

/**
 * @brief perspective implements GLUPerspective
 * @param fovy is fov angle
 * @param aspect is screen aspect ration
 * @param zNear is near cutting plate
 * @param zFar is far cutting plane
 */
void gles20::perspective(float fovy, float aspect, float zNear, float zFar) {
    while(!matrixBuffer.empty()) {
        matrixBuffer.pop();
    }
    proj_matrix = glm::perspective((float)(fovy * M_PI / 180.0f), aspect, zNear,zFar);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(true);
}

/**
 * @brief multMatrix multiplies with matrix
 * @param matrix is 4x4 matrix in OpenGL format
 */
void gles20::multMatrix(float* matrix) {
  matrix_result *= glm::mat4x4(
      matrix[0], matrix[1], matrix[2], matrix[3],
      matrix[4], matrix[5], matrix[6], matrix[7],
      matrix[8], matrix[9], matrix[10], matrix[11],
      matrix[12], matrix[13], matrix[14], matrix[15]
  );
}

/**
 * @brief popMatrix pops matrix from stack
 */
void gles20::popMatrix() {
  /// popping matrix from stack
  matrix_result = (glm::mat4x4)matrixBuffer.top();
  matrixBuffer.pop();
}

/**
 * @brief pushMatrix pushs current matrix to stack
 */
void gles20::pushMatrix() {
  /// push matrix m to stack
  matrixBuffer.push(matrix_result);
}

/**
 * @brief rotateX rotate around X axis
 * @param value is angle
 */
void gles20::rotateX(float value) {
  float radian = value * M_PI / 180;
  /// rotation matrix for 2D transformations (around Z axis)
  glm::mat4x4 rotation(
      1,0,0,0,
      0,cosf(radian),-sinf(radian),0,
      0,sinf(radian),cosf(radian),0,
      0,0,0,1
  );
  /// mult current maxtrix
  matrix_result *= rotation;
}

/**
 * @brief rotateX rotate around Y axis
 * @param value is angle
 */
void gles20::rotateY(float value) {
  float radian = value * M_PI / 180;
  /// rotation matrix for 2D transformations (around Z axis)
  glm::mat4x4 rotation(
      cosf(radian),0,-sinf(radian),0,
      0,1,0,0,
      sinf(radian),0,cosf(radian),0,
      0,0,0,1
  );
  /// mult current maxtrix
  matrix_result *= rotation;
}

/**
 * @brief rotateX rotate around Z axis
 * @param value is angle
 */
void gles20::rotateZ(float value) {
  float radian = value * M_PI / 180;
  /// rotation matrix for 2D transformations (around Z axis)
  glm::mat4x4 rotation(
      cosf(radian),sinf(radian),0,0,
      -sinf(radian),cosf(radian),0,0,
      0,0,1,0,
      0,0,0,1
  );
  /// mult current maxtrix
  matrix_result *= rotation;
}

/**
 * @brief scale scales current matrix
 * @param value is amount of scale(1 to keep current)
 */
void gles20::scale(float value) {
  /// scale matrix
  glm::mat4x4 scale(
      value,0,0,0,
      0,value,0,0,
      0,0,value,0,
      0,0,0,1
  );
  /// mult current maxtrix
  matrix_result *= scale;
}

/**
 * @brief translate translates object
 * @param x is translate coordinate
 * @param y is translate coordinate
 * @param z is translate coordinate
 */
void gles20::translate(float x, float y, float z) {
  /// transformation matrix
  glm::mat4x4 translation(
      1,0,0,0,
      0,1,0,0,
      0,0,1,0,
      x,y,z,1
  );
  /// mult current matrix
  matrix_result *= translation;
}

/**
 * @brief renderDynamic render dynamic objects
 * @param geom is geometry vbo
 * @param sh is shader to use
 * @param txt is texture to use
 * @param triangleCount is triangle count
 */
void gles20::renderDynamic(float* vertices, float* normals, float* coords, shader* sh, texture* txt, int triangleCount) {

    /// set OpenGL state
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    glDisable(GL_CULL_FACE);
    txt->apply();

    /// set matrices
    sh->bind();
    matrix = proj_matrix * view_matrix;
    sh->uniformMatrix("u_Matrix",glm::value_ptr(matrix));

    /// render
    sh->attrib(vertices, normals, coords);
    glDrawArrays(GL_TRIANGLES, 0, triangleCount * 3);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    /// set previous OpenGL state
    sh->unbind();
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

/**
 * @brief renderModel renders model into scene
 * @param m is instance of model to render
 */
void gles20::renderModel(model* m)
{
    // culled model
    if (!m->v3d.empty())
    {
        id3d id;
        int steps = 2;
        int cx = camera.x / CULLING_DST;
        int cy = camera.y / CULLING_DST;
        int cz = camera.z / CULLING_DST;
        for (id.x = cx - steps; id.x <= cx + steps; id.x++)
            for (id.y = cy - steps; id.y <= cy + steps; id.y++)
                for (id.z = cz - steps; id.z <= cz + steps; id.z++)
                {
                    glm::vec3 part = glm::vec3(id.x - cx, id.y - cy, id.z - cz);
                    if ((glm::dot(part, direction) > -0.005f) ||
                        (fabs(part.x) < 1.5f) || (fabs(part.x) < 1.5f) || (fabs(part.x) < 1.5f))
                        if (m->v3d.find(id) != m->v3d.end())
                            for (unsigned int i = 0; i < m->v3d[id].size(); i++)
                            {
                                current = m->v3d[id][i].material;
                                current->bind();
                                if (m->v3d[id][i].texture2D->transparent)
                                    glDisable(GL_CULL_FACE);
                                else
                                    glEnable(GL_CULL_FACE);
                                renderSubModel(&m->v3d[id][i]);
                                current->unbind();
                            }
                }
    }

    /// set opengl for rendering models
    for (unsigned int i = 0; i < m->models.size(); i++)
        if (enable[m->models[i].filter] && !m->models[i].touchable)
            if(m->models[i].dynamic || m->v3d.empty())
            {
                current = m->models[i].material;
                current->bind();
                if (m->models[i].texture2D->transparent)
                    glDisable(GL_CULL_FACE);
                else
                    glEnable(GL_CULL_FACE);
                renderSubModel(&m->models[i]);
                current->unbind();
            }
    glEnable(GL_CULL_FACE);
}


/**
 * @brief renderShadow renders shadow of model into scene
 * @param m is instance of model to render
 */
void gles20::renderShadow(model* m) {

    if (!rtt_fbo[oddFrame]->complete || !m->v3d.empty())
        return;

    glDepthMask(false);
    glEnable(GL_BLEND);
    glEnable(GL_STENCIL_TEST);
    glBlendFunc(GL_ONE, GL_ONE);
    glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
    glStencilMask(true);
    current = shadow;
    current->bind();
    current->uniformFloat4("u_sun_dir", -1.5f, -3.0f, 0.0f, 0.0f);
    for (int pass = 1; pass <= 1; pass++) {
        /// set visibility shape
        current->uniformFloat("u_pass", pass * 0.05f);
        current->uniformFloat("u_offset", 0.0f);
        glColorMask(false, false, false, false);
        glDepthFunc(GL_LEQUAL);
        glStencilFunc(GL_ALWAYS, 0, 1);
        glStencilOp(GL_KEEP,GL_KEEP,GL_ZERO);
        for (unsigned int i = 0; i < m->models.size(); i++)
            if (!m->models[i].filter)
                renderSubModel(&m->models[i]);

        /// render shadow
        current->uniformFloat("u_offset", 0.5f);
        glColorMask(true, true, true, true);
        glDepthFunc(GL_GEQUAL);
        glStencilFunc(GL_EQUAL, 0, 1);
        glStencilOp(GL_KEEP,GL_KEEP, GL_INCR);
        for (unsigned int i = 0; i < m->models.size(); i++)
            if (!m->models[i].filter)
                renderSubModel(&m->models[i]);
    }

    /// set up previous state
    current->unbind();
    glBlendEquation(GL_FUNC_ADD);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(true);
    glDisable(GL_CULL_FACE);
    glDisable(GL_STENCIL_TEST);
    glStencilMask(false);
}

/**
 * @brief renderSubModel renders model into scene
 * @param m is instance of model to render
 */
void gles20::renderSubModel(model3d *m) {

    /// set model matrix
    glm::mat4x4 modelView;
    if (m->dynamic) {
        float mat[16];
        getPhysics()->getTransform(m->dynamicID, mat);
        float w = m->reg.max.x - m->reg.min.x;
        float a = m->reg.max.y - m->reg.min.y;
        float h = m->reg.max.z - m->reg.min.z;
        glm::mat4x4 translation(
            1,0,0,0,
            0,1,0,0,
            0,0,1,0,
            -w/2, -a/2, -h/2, 1
        );
        glm::mat4x4 dynamic(
            mat[0],mat[1],mat[2],mat[3],
            mat[4],mat[5],mat[6],mat[7],
            mat[8],mat[9],mat[10],mat[11],
            mat[12],mat[13],mat[14],mat[15]
        );
        modelMat = dynamic * translation;
        modelView = view_matrix * modelMat;
    } else {
        glm::mat4x4 translation(
            1,0,0,0,
            0,1,0,0,
            0,0,1,0,
            m->reg.min.x, m->reg.min.y, m->reg.min.z, 1
        );
        modelMat = matrix_result * translation;
        modelView = view_matrix * modelMat;
    }
    glm::mat4x4 projView = proj_matrix * view_matrix;

    /// set matrices
    current->uniformMatrix("u_ModelMatrix",glm::value_ptr(modelMat));
    current->uniformMatrix("u_ViewMatrix",glm::value_ptr(view_matrix));
    current->uniformMatrix("u_ModelViewMatrix",glm::value_ptr(modelView));
    current->uniformMatrix("u_ProjViewMatrix",glm::value_ptr(projView));
    current->uniformMatrix("u_ProjectionMatrix",glm::value_ptr(proj_matrix));
    matrix = proj_matrix * modelView;
    current->uniformMatrix("u_Matrix",glm::value_ptr(matrix));

    /// previous screen
    glActiveTexture( GL_TEXTURE1 );
    rtt_fbo[oddFrame]->bindTexture();
    current->uniformInt("EnvMap1", 1);

    /// set texture
    glActiveTexture( GL_TEXTURE0 );
    m->texture2D->apply();
    current->uniformInt("color_texture", 0);

    /// set uniforms
    current->uniformFloat("u_width", 1 / (float)screen_width / aliasing);
    current->uniformFloat("u_height", 1 / (float)screen_height / aliasing);
    current->uniformFloat4("u_kA", m->colora[0], m->colora[1], m->colora[2], 1);
    current->uniformFloat4("u_kD", m->colord[0], m->colord[1], m->colord[2], 1);
    current->uniformFloat4("u_kS", m->colors[0], m->colors[1], m->colors[2], 1);
    if (enable[9])
        current->uniformFloat("u_brake", 1);
    else
        current->uniformFloat("u_brake", 0);

    current->attrib(&m->vertices[0], &m->normals[0], &m->coords[0]);
    glDrawArrays(GL_TRIANGLES, 0, m->vertices.size() / 3);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/**
 * @brief rtt enables rendering into FBO which makes posible to do reflections
 * @param enable is true to start drawing, false to render on screen
 */
void gles20::rtt(bool enable) {
    if (enable) {
#ifndef ANDROID
        /// start timer
        glGenQueries(1,gpuMeasuring);
        glBeginQuery(GL_TIME_ELAPSED, gpuMeasuring[0]);
#endif
        rtt_fbo[oddFrame]->bindFBO();
        rtt_fbo[oddFrame]->clear();
        oddFrame = !oddFrame;
    } else {
#ifndef ANDROID
        glEndQuery(GL_TIME_ELAPSED);

        /// get scene counters
        GLint gpu_time = 0;
        glGetQueryObjectiv(gpuMeasuring[0], GL_QUERY_RESULT, &gpu_time);

        GLint copy_time = 0;
        glBeginQuery(GL_TIME_ELAPSED, gpuMeasuring[0]);
#endif
        /// rendering
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport (0, 0, screen_width, screen_height);
        rtt_fbo[oddFrame]->drawOnScreen(scene_shader);

#ifndef ANDROID
        glEndQuery(GL_TIME_ELAPSED);
        glGetQueryObjectiv(gpuMeasuring[0], GL_QUERY_RESULT, &copy_time);
        printf("3D time: %dk 2D time: %dk, edge:%d, dst:%f\n", gpu_time / 1000,
               copy_time / 1000, getCar(0)->currentEdgeIndex, getCar(0)->toFinish);
#endif
    }
}
