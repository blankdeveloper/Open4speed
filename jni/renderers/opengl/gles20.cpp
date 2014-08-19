//----------------------------------------------------------------------------------------
/**
 * \file       gles20.cpp
 * \author     Vonasek Lubos
 * \date       2014/02/14
 * \brief      GL renderer draws geometry and other things on screen
*/
//----------------------------------------------------------------------------------------

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stack>
#include "loaders/pngloader.h"
#include "loaders/rgb.h"
#include "renderers/opengl/gles20.h"
#include "renderers/opengl/glfbo.h"
#include "renderers/opengl/gltexture.h"
#include "utils/io.h"
#include "utils/math.h"
#include "utils/switch.h"
#include "common.h"

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
    camX = 0;
    camY = 0;
    camZ = 0;
    frame = 0;
    oddFrame = true;

    /// set open-gl
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glDepthFunc(GL_LEQUAL);

    //set shaders
    scene_shader = getShader("scene");
    shadow = getShader("shadow");

    //find ideal texture resolution
    int resolution = 2;
    while (resolution < screen_width) {
        resolution *= 2;
    }

    //create render texture
    for (int i = 0; i < 2; i++) {
        rtt[i] = new glfbo(screen_width, screen_height);
    }

    //set viewport
    glViewport (0, 0, (GLsizei) screen_width, (GLsizei) screen_height);
    clear(true);
    glActiveTexture( GL_TEXTURE0 );
}

/**
 * @brief lookAt implements GLUlookAt
 * @param eyex is eye vector coordinate
 * @param eyey is eye vector coordinate
 * @param eyez is eye vector coordinate
 * @param centerx is camera center coordinate
 * @param centery is camera center coordinate
 * @param centerz is camera center coordinate
 * @param upx is up vector coordinate
 * @param upy is up vector coordinate
 * @param upz is up vector coordinate
 */
void gles20::lookAt(GLfloat eyex, GLfloat eyey, GLfloat eyez,
                     GLfloat centerx, GLfloat centery, GLfloat centerz,
                     GLfloat upx, GLfloat upy, GLfloat upz) {

    camX = eyex;
    camY = eyey;
    camZ = eyez;
    view_matrix = glm::lookAt(glm::vec3(eyex, eyey, eyez),
                              glm::vec3(centerx, centery, centerz),
                              glm::vec3(upx, upy, upz));
    matrix_result = eye;
}

/**
 * @brief perspective implements GLUPerspective
 * @param fovy is fov angle
 * @param aspect is screen aspect ration
 * @param zNear is near cutting plate
 * @param zFar is far cutting plane
 */
void gles20::perspective(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar) {
    if(!matrixBuffer.empty()) {
        matrixBuffer.pop();
    }
    proj_matrix = glm::perspective(fovy, aspect, zNear,zFar);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(true);
}

/**
 * @brief clear clears fragmet/depth buffer
 * @param colors true to clear both, false to clear only depth buffer
 */
void gles20::clear(bool colors) {
    if (colors) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    } else {
        glClear(GL_DEPTH_BUFFER_BIT);
    }
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
 * @param vertices is vertices
 * @param coords is texture coords
 * @param sh is shader to use
 * @param txt is texture to use
 * @param triangleCount is triangle count
 */
void gles20::renderDynamic(GLfloat *vertices, GLfloat *coords, shader* sh, texture* txt, int triangleCount) {

    /// init OpenGL state
    glEnable(GL_BLEND);
    glDepthMask(false);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    glDisable(GL_CULL_FACE);
    txt->apply();

    /// set matrices
    sh->bind();
    matrix = proj_matrix * view_matrix;
    sh->uniformMatrix("u_Matrix",glm::value_ptr(matrix));

    /// render
    sh->attrib(vertices, coords);
    glDrawElements(GL_TRIANGLES, triangleCount * 3, GL_UNSIGNED_SHORT, dynindices);

    /// set previous OpenGL state
    sh->unbind();
    glDisable(GL_BLEND);
    glDepthMask(true);
}

/**
 * @brief renderModel renders model into scene
 * @param m is instance of model to render
 * @param physic is physical model instance
 * @param gamma is requested render gamma
 */
void gles20::renderModel(model* m) {

    /// set culling info positions
    xm = (camX - m->aabb.min.x) / culling;
    xp = xm;
    ym = (camZ - m->aabb.min.z) / culling;
    yp = ym;
    for (float view = direction - M_PI * 0.75f; view <= direction + M_PI * 0.75f; view += M_PI * 0.25f)
    {
        if (xp < (camX - m->aabb.min.x) / culling + sin(view) * 2)
            xp = (camX - m->aabb.min.x) / culling + sin(view) * 2;
        if (xm > (camX - m->aabb.min.x) / culling + sin(view) * 2)
            xm = (camX - m->aabb.min.x) / culling + sin(view) * 2;
        if (yp < (camZ - m->aabb.min.z) / culling + cos(view) * 2)
            yp = (camZ - m->aabb.min.z) / culling + cos(view) * 2;
        if (ym > (camZ - m->aabb.min.z) / culling + cos(view) * 2)
            ym = (camZ - m->aabb.min.z) / culling + cos(view) * 2;
    }
    if (xm < 0)
        xm = 0;
    if (ym < 0)
        ym = 0;
    if (xp >= m->cutX)
        xp = m->cutX - 1;
    if (yp >= m->cutY)
        yp = m->cutY - 1;

    /// set opengl for rendering models
    for (unsigned int i = 0; i < m->models.size(); i++) {
        current = m->models[i].material;
        current->bind();
        if (!m->models[i].texture2D->transparent)
            if (enable[m->models[i].filter]) {

                /// set alpha channel
                if ((m->models[i].texture2D->alpha < 0.9) || (m->models[i].texture2D->transparent)) {
                    glEnable(GL_BLEND);
                    glDepthMask(false);
                    glDisable(GL_CULL_FACE);
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    current->uniformFloat("u_Alpha", m->models[i].texture2D->alpha);
                } else {
                    glEnable(GL_CULL_FACE);
                    glCullFace(GL_BACK);
                    glDisable(GL_BLEND);
                    glDepthMask(true);
                    current->uniformFloat("u_Alpha", 1);
                }

                renderSubModel(m, &m->models[i]);
            }
    }
    for (unsigned int i = 0; i < m->models.size(); i++) {
        current = m->models[i].material;
        current->bind();
        if (m->models[i].texture2D->transparent)
            if (enable[m->models[i].filter]) {

                /// set alpha channel
                if ((m->models[i].texture2D->alpha < 0.9) || (m->models[i].texture2D->transparent)) {
                    glEnable(GL_BLEND);
                    glDepthMask(false);
                    glDisable(GL_CULL_FACE);
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    current->uniformFloat("u_Alpha", m->models[i].texture2D->alpha);
                } else {
                    glEnable(GL_CULL_FACE);
                    glCullFace(GL_BACK);
                    glDisable(GL_BLEND);
                    glDepthMask(true);
                    current->uniformFloat("u_Alpha", 1);
                }

                renderSubModel(m, &m->models[i]);
            }
    }
}


/**
 * @brief renderShadow renders shadow of model into scene
 * @param m is instance of model to render
 */
void gles20::renderShadow(model* m) {

    /// set culling info positions
    xm = (camX - m->aabb.min.x) / culling;
    xp = xm;
    ym = (camZ - m->aabb.min.z) / culling;
    yp = ym;
    for (float view = direction - M_PI * 0.75f; view <= direction + M_PI * 0.75f; view += M_PI * 0.25f)
    {
        if (xp < (camX - m->aabb.min.x) / culling + sin(view) * 2)
            xp = (camX - m->aabb.min.x) / culling + sin(view) * 2;
        if (xm > (camX - m->aabb.min.x) / culling + sin(view) * 2)
            xm = (camX - m->aabb.min.x) / culling + sin(view) * 2;
        if (yp < (camZ - m->aabb.min.z) / culling + cos(view) * 2)
            yp = (camZ - m->aabb.min.z) / culling + cos(view) * 2;
        if (ym > (camZ - m->aabb.min.z) / culling + cos(view) * 2)
            ym = (camZ - m->aabb.min.z) / culling + cos(view) * 2;
    }
    if (xm < 0)
        xm = 0;
    if (ym < 0)
        ym = 0;
    if (xp >= m->cutX)
        xp = m->cutX - 1;
    if (yp >= m->cutY)
        yp = m->cutY - 1;

    //set openGL function
    glDepthMask(false);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_STENCIL_TEST);

    //render shadow
    glClearStencil(0x0);
    glClear(GL_STENCIL_BUFFER_BIT);
    glStencilFunc(GL_EQUAL, 0, 0xFF);
    glStencilOp(GL_KEEP,GL_KEEP,GL_INCR);
    for (unsigned int i = 0; i < m->models.size(); i++) {
        current = shadow;
        current->bind();
        if (!m->models[i].texture2D->transparent)
            if (enable[m->models[i].filter])
                renderSubModel(m, &m->models[i]);
    }


    ///remove unwanted shadow
    glBlendEquation(GL_FUNC_ADD);
    glCullFace(GL_FRONT);
    glStencilOp(GL_KEEP,GL_KEEP,GL_DECR);
    for (unsigned int i = 0; i < m->models.size(); i++) {
        current = shadow;
        current->bind();
        if (!m->models[i].texture2D->transparent)
            if (enable[m->models[i].filter])
                renderSubModel(m, &m->models[i]);
    }
    glDisable(GL_STENCIL_TEST);
}

/**
 * @brief renderSubModel renders model into scene
 * @param m is instance of model to render
 */
void gles20::renderSubModel(model* mod, model3d *m) {

    /// set model matrix
    glm::mat4x4 modelView;
    if (m->dynamic) {
        physic->getTransform(m->dynamicID, mat);
        m->x = mat[12];
        m->y = mat[13];
        m->z = mat[14];
        float w = m->reg->max.x - m->reg->min.x;
        float a = m->reg->max.y - m->reg->min.y;
        float h = m->reg->max.z - m->reg->min.z;
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
            m->reg->min.x, m->reg->min.y, m->reg->min.z,1
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
    rtt[oddFrame]->bindTexture();
    current->uniformInt("EnvMap1", 1);

    /// set texture
    glActiveTexture( GL_TEXTURE0 );
    m->texture2D->apply();
    current->uniformInt("color_texture", 0);

    /// set uniforms
    glm::vec4 mpos = (proj_matrix * view_matrix * model_position);
    float z = mpos.z;
    mpos /= mpos.w;
    float y = mpos.y * 0.5 + 0.5 - 2 / z;
    current->uniformFloat("u_Time", frame / 1000.0f);
    current->uniformFloat4("u_model_position", glm::clamp(mpos.x * 0.5f + 0.5f, 0.0f, 1.0f), glm::max(0.0f, y), 0, 1);
    current->uniformFloat("u_width", 1 / (float)screen_width);
    current->uniformFloat("u_height", 1 / (float)screen_height);
    current->uniformFloat4("camera", camX, camY, camZ, 1);
    current->uniformFloat4("u_kA", m->colora[0], m->colora[1], m->colora[2], 1);
    current->uniformFloat4("u_kD", m->colord[0], m->colord[1], m->colord[2], 1);
    current->uniformFloat4("u_kS", m->colors[0], m->colors[1], m->colors[2], 1);
    current->uniformFloat("u_speed", allCar[cameraCar]->speed / 500.0f + 0.35f);
    current->uniformFloat("u_gamma", 1.0);
    if (enable[9])
        current->uniformFloat("u_brake", 1);
    else
        current->uniformFloat("u_brake", 0);

    /// post light as uniform into shader
    current->uniformFloat4("u_light_diffuse", light.u_light_diffuse.x, light.u_light_diffuse.y, light.u_light_diffuse.z, 1.0);
    current->uniformFloat("u_light_cut", light.u_light_cut);
    current->uniformFloat("u_light_near", light.u_near);
    current->uniformFloat("u_light_spot_eff", light.u_light_spot_eff);
    current->uniformFloat4("u_light", light.u_light.x, light.u_light.y, light.u_light.z, 1.0);
    current->uniformFloat4("u_light_att", light.u_light_att.x, light.u_light_att.y, light.u_light_att.z, 1.0);
    current->uniformFloat4("u_light_dir", light.u_light_dir.x, light.u_light_dir.y, light.u_light_dir.z, 1.0);
    current->uniformFloat4("u_nearest1", light.u_nearest1.x, light.u_nearest1.y, light.u_nearest1.z, 1.0);

    if (current->shadername[0] == '0') {
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
    }

    /// standart vertices
    if (mod->cutX * mod->cutY == 1) {
        m->vboData->render(current, 0, m->triangleCount[mod->cutX * mod->cutY]);
    }

    /// culled vertices
    else {
        for (int i = ym; i <= yp; i++) {
            int l = m->triangleCount[i * mod->cutX + xm];
            int r = m->triangleCount[i * mod->cutX + xp + 1];
            m->vboData->render(current, l, r - l);
        }
        int l = m->triangleCount[(mod->cutX - 1) * mod->cutY];
        int r = m->triangleCount[mod->cutX * mod->cutY];
        m->vboData->render(current, l, r - l);
    }

    //unbind shader
    current->unbind();
}
