//----------------------------------------------------------------------------------------
/**
 * \file       gles20.cpp
 * \author     Vonasek Lubos
 * \date       2014/01/05
 * \brief      GL renderer draws geometry and other things on screen
*/
//----------------------------------------------------------------------------------------

#include "stdafx.h"

GLushort dynindices[4095];      ///< Indicies for dynamic rendering

bool renderShadowMap = false;   ///< Special state for rendering shadowmap
int culling = 300;              ///< View culling distance in meters
float camX;                     ///< Camera position x
float camY;                     ///< Camera position y
float camZ;                     ///< Camera position z

int lmFilter = -1;              ///< Filtering of objects to render
const int rttsize = 2048;       ///< RTT texture size
texture *glslfont = 0;          ///< Font texture
shader* gui_shader = 0;         ///< GUI shader
shader* scene_shader = 0;       ///< Scene shader
shader* shadowmap = 0;          ///< Shadowmap shader

/**
 * Lightmap RTTs
 */
RTT cube[6];                    ///< Cubemap framebuffer
RTT* lm;                        ///< Lightmap framebuffer

/**
 * GUI projection matrix
 */
glm::mat4x4 gui_projection_matrix(
    (float)(1 / tan(45 * (3.1415926535 / 180))), 0, 0, 0,
    0, (float)(1 / tan(45 * (3.1415926535 / 180))), 0, 0,
    0, 0, -1, -1,
    0, 0, -1, 0
);

/**
 * Eye matrix
 */
glm::mat4x4 eye = glm::mat4x4(
            1,0,0,0,
            0,1,0,0,
            0,0,1,0,
            0,0,0,1
);

/**
 * Bias matrix to transform world coordinates into texture coordinates
 */
glm::mat4 matScale = glm::mat4(0.5f, 0.0f, 0.0f, 0.0f,
                               0.0f, 0.5f, 0.0f, 0.0f,
                               0.0f, 0.0f, 0.5f, 0.0f,
                               0.5f, 0.5f, 0.5f, 1.0f);

glm::mat4x4 matrix;                                                        ///< Matrix for dynamic rendering
glm::mat4x4 matrixScl;                                                     ///< Matrix for shadow mapping
glm::mat4x4 scene_projection_matrix;                                       ///< Scene projection matrix
glm::mat4x4 view_matrix;                                                   ///< View matrix
glm::mat4x4 matrix_result;                                                 ///< Temp matrix for calculations
std::stack<glm::mat4x4> matrixBuffer = *(new std::stack<glm::mat4x4>());   ///< Matrix stack

/**
  * Enabling and disabling lamps in time
  */
bool lamp[] = {0,0,0,0,1,0,1,1,0,1,
               1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,0,0,
               1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,0,0,1,0,1};

/**
 * @brief initRTT inits framebuffer and renderbuffer
 * @param rtt is structure of framebuffer and renderbuffer together
 */
void initRTT(RTT *rtt) {

    //create frame buffer
    glGenFramebuffers(1, &rtt->fboID);
    glBindFramebuffer(GL_FRAMEBUFFER, rtt->fboID);
    glGenTextures(1, &rtt->rendertexture);
    glBindTexture(GL_TEXTURE_2D, rtt->rendertexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rtt->res, rtt->res, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rtt->rendertexture, 0);

    //create texture for depth buffer
    if (renderLightmap) {
        glGenTextures(1, &rtt->rendertexture2);
        glBindTexture(GL_TEXTURE_2D, rtt->rendertexture2);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, rtt->res, rtt->res, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, rtt->rendertexture2, 0);
    }
    //create render buffer
    else {
        glGenRenderbuffers(1, &rtt->rboID);
        glBindRenderbuffer(GL_RENDERBUFFER, rtt->rboID);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, rtt->res, rtt->res);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rtt->rboID);
    }


    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        exit(1);

    //clear
    glViewport (0, 0, rtt->width, rtt->height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
    overmode = 0;
    overshader = 0;
    frame = 0;
    oddFrame = true;

    /// set open-gl
    if (renderLightmap)
        glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    else
        glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glDepthFunc(GL_LESS);
    glDisable(GL_CULL_FACE);

    //set shaders
    gui_shader = getShader("gui");
    scene_shader = getShader("scene");
    shadowmap = getShader("shadowmap");

    //find ideal texture resolution
    int resolution = 2;
    while (resolution < screen_width * antialiasing) {
        resolution *= 2;
    }

    //create render texture
    for (int i = 0; i < 2; i++) {
        rtt[i] = *(new RTT());
        rtt[i].res = resolution;
        rtt[i].width = screen_width * antialiasing;
        rtt[i].height = screen_height * antialiasing;
        initRTT(&rtt[i]);
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
 * @brief lookAt implements GLUlookAt without up vector
 * @param eyex is eye vector coordinate
 * @param eyey is eye vector coordinate
 * @param eyez is eye vector coordinate
 * @param centerx is camera center coordinate
 * @param centery is camera center coordinate
 * @param centerz is camera center coordinate
 */
void gles20::lookAt(GLfloat eyex, GLfloat eyey, GLfloat eyez,
                     GLfloat centerx, GLfloat centery, GLfloat centerz) {

    camX = eyex;
    camY = eyey;
    camZ = eyez;
    float upx = 0;
    float upy = 0;
    float upz = 0;

    /// set up vector
    if (absf(eyex - centerx) > absf(eyey - centery)) {
        upy = 1;
    } else if (absf(eyex - centerx) == absf(eyey - centery)) {
        if (absf(eyex - centerx) > absf(eyez - centerz)) {
            upz = 1;
        } else {
            upx = 1;
        }
    } else {
        upx = 1;
    }


    /// set matrices
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
    scene_projection_matrix = glm::perspective(fovy, aspect, zNear,zFar);
    glEnable(GL_DEPTH_TEST);
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
 * @brief renderButton renders button in GUI mode
 * @param x is position x
 * @param y is position y
 * @param w is width
 * @param h is height
 * @param layer is distance from camera
 * @param button is button texture
 * @param text is button text
 */
void gles20::renderButton(float x, float y, float w, float h, float layer, texture* button, const char* text) {
    renderImage(x, y, w, h, layer, button);
    renderText(x, y, layer, text);
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
    matrix = scene_projection_matrix * view_matrix;
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
 * @brief renderImage renders image in GUI mode
 * @param x is position x
 * @param y is position y
 * @param w is width
 * @param h is height
 * @param layer is distance from camera
 * @param image is image texture
 */
void gles20::renderImage(float x, float y, float w, float h, float layer, texture* image) {
    /// indicies
    GLubyte indices[] = {3,0,1,3,1,2};

    /// vertices
    GLfloat vertices[] = {
        -w / 100, -h / 100, 0,
        +w / 100, -h / 100, 0,
        +w / 100, +h / 100, 0,
        -w / 100, +h / 100, 0
    };

    GLfloat coords[] = {0,0,1,0,1,1,0,1};

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    image->apply();

    /// render
    glm::mat4x4 gui_modelview_matrix(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        w / 100 - 1 + x / 50, -h / 100 + 1 - y / 50, -layer, 1
    );
    // mult current maxtrix
    matrix_result = gui_projection_matrix * gui_modelview_matrix;

    gui_shader->bind();
    gui_shader->uniformMatrix("u_Matrix", glm::value_ptr(matrix_result));
    gui_shader->attrib(vertices, coords);
    glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLubyte), GL_UNSIGNED_BYTE, indices);
    glDisable(GL_BLEND);
    gui_shader->unbind();
}

/**
 * @brief renderModel renders model into scene
 * @param m is instance of model to render
 * @param physic is physical model instance
 * @param gamma is requested render gamma
 */
void gles20::renderModel(model* m) {
    glDisable(GL_BLEND);
    glActiveTexture(GL_TEXTURE0);
    /// set opengl for rendering models
    for (unsigned int i = 0; i < m->models.size(); i++) {
        if (enable[m->models[i].filter] && ((lmFilter < 0) || (m->models[i].lmIndex == lmFilter))) {
            renderSubModel(m, &m->models[i]);
        }
    }
}

/**
 * @brief renderSubModel renders model into scene
 * @param m is instance of model to render
 * @param physic is physical model instance
 * @param gamma is requested render gamma
 */
void gles20::renderSubModel(model* mod, model3d *m) {

    /// set model matrix
    glm::mat4x4 modelView;
    if (m->dynamic) {
        physic->getTransform(m->dynamicID, mat);
        m->x = mat[12];
        m->y = mat[13];
        m->z = mat[14];
        float w = m->reg->maxX - m->reg->minX;
        float a = m->reg->maxY - m->reg->minY;
        float h = m->reg->maxZ - m->reg->minZ;
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
        modelView = view_matrix * dynamic * translation;
    } else {
        glm::mat4x4 translation(
            1,0,0,0,
            0,1,0,0,
            0,0,1,0,
            m->reg->minX, m->reg->minY, m->reg->minZ,1
        );
        modelView = view_matrix * matrix_result * translation;
    }

    /// set texture
    m->texture2D->apply();

    /// set depth test
    if (lmFilter < 0) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }

    /// set special cases
    if (renderShadowMap && !m->texture2D->transparent) {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
    } else if (m->texture2D->transparent || (lmFilter >= 0)) {
        glDisable(GL_CULL_FACE);
    } else {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }

    /// set shader and VBO
    glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
    shader* current = m->material;
    if (overshader != 0) {
        current = overshader;
    }

    /// set matrices
    current->bind();
    current->uniformMatrix("u_ModelViewMatrix",glm::value_ptr(modelView));
    current->uniformMatrix("u_ProjectionMatrix",glm::value_ptr(scene_projection_matrix));
    matrix = scene_projection_matrix * modelView;
    matrixScl = matScale * matrix;
    current->uniformMatrix("u_Matrix",glm::value_ptr(matrix));
    current->uniformMatrix("u_MatrixScl",glm::value_ptr(matrixScl));

    /// apply lightmap
    if (m->lightmap != 0) {
        glActiveTexture( GL_TEXTURE3 );
        m->lightmap->setFrame(!lamp[frame % 100]);
        m->lightmap->apply();
    }

    /// previous screen
    if (!renderLightmap) {
        glActiveTexture( GL_TEXTURE1 );
        glBindTexture(GL_TEXTURE_2D, rtt[oddFrame].rendertexture);
    }
    glActiveTexture( GL_TEXTURE0 );

    /// set samplers
    current->uniformInt("color_texture", 0);
    current->uniformInt("EnvMap1", 1);
    current->uniformInt("Shadowmap", 2);
    current->uniformInt("Lightmap", 3);

    /// set uniforms
    current->uniformFloat("u_Time", frame / 1000.0f);
    current->uniformFloat("u_res", 1 / (float)rtt[oddFrame].res);
    current->uniformFloat("u_width", 1 / (float)screen_width);
    current->uniformFloat("u_height", 1 / (float)screen_height);
    current->uniformFloat4("camera", camX, camY, camZ, 1);
    current->uniformFloat("u_test", testUniform);
    current->uniformFloat("u_view", rtt[oddFrame].height / (float)rtt[oddFrame].res);
    current->uniformFloat4("u_kA", m->colora[0], m->colora[1], m->colora[2], 1);
    current->uniformFloat4("u_kD", m->colord[0], m->colord[1], m->colord[2], 1);
    current->uniformFloat4("u_kS", m->colors[0], m->colors[1], m->colors[2], 1);
    current->uniformFloat("u_speed", allCar[cameraCar]->speed / 500.0f + 0.35f);
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

    /// set alpha channel
    /*if ((m->texture2D->alpha < 0.9) || (m->texture2D->transparent)) {
        glEnable(GL_BLEND);
        glDepthMask(false);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE);
        current->uniformFloat("u_Alpha", m->texture2D->alpha);
    } else {
        glDisable(GL_BLEND);
        current->uniformFloat("u_Alpha", 1);
    }*/

    if (overmode == 0) {
        glDepthMask(true);
        glDisable(GL_BLEND);
    } else if (overmode == 1) {
        glDepthMask(false);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
    }
    if (current->shadername[0] == '0') {
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
    }

    /// set culling info positions
    int xm = (camX - mod->minx) / culling - 1;
    int xp = xm + 2;
    int ym = (camZ - mod->minz) / culling - 1;
    int yp = ym + 2;
    if (xm < 0)
        xm = 0;
    if (ym < 0)
        ym = 0;
    if (xp >= mod->cutX)
        xp = mod->cutX - 1;
    if (yp >= mod->cutY)
        yp = mod->cutY - 1;

    current->attrib(sizeof(GLfloat) * m->triangleCount[mod->cutX * mod->cutY] * 3);

    /// standart vertices
    if (mod->cutX * mod->cutY == 1) {
        glDrawArrays(GL_TRIANGLES, 0, m->triangleCount[mod->cutX * mod->cutY] * 3);
    }

    /// culled vertices
    else {
        for (int i = ym; i <= yp; i++) {
            int l = m->triangleCount[i * mod->cutX + xm] * 3;
            int r = m->triangleCount[i * mod->cutX + xp + 1] * 3;
            glDrawArrays(GL_TRIANGLES, l, r - l);
        }
        int l = m->triangleCount[(mod->cutX - 1) * mod->cutY] * 3;
        int r = m->triangleCount[mod->cutX * mod->cutY] * 3;
        glDrawArrays(GL_TRIANGLES, l, r - l);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //unbind shader
    glUseProgram(0);
    glDepthMask(true);
    glDisable(GL_BLEND);
}

/**
 * @brief renderText renders text in GUI mode
 * @param x is position x
 * @param y is position y
 * @param layer is distance from camera
 * @param text is button text
 */
void gles20::renderText(float x, float y, float layer, const char* text) {
    int minus = 0;

    /// load font
    if (glslfont == 0) {
        glslfont = getTexture(fontTexture, 1);
    }

    /// set OpenGL
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /// draw characters
    for (unsigned int i = 0; i < strlen(text); i++) {
        /// indicies
        GLubyte indices[] = {3,0,1,3,1,2};

        /// vertices
        GLfloat vertices[] = {
            x / 50 - 1, 1 - y / 50 - 1, 0,
            x / 50, 1 - y / 50 - 1, 0,
            x / 50, 1 - y / 50, 0,
            x / 50 - 1, 1 - y / 50, 0
        };

        /// new line
        if (text[i] == '\\') {
            minus = i + 1;
            y += 5;
        }

        /// still same line
        else {

            /// get character position
            int cx = text[i] % (int)fontRows;
            int cy = text[i] / (int)fontRows;
            float size = 1 / fontRows;

            /// set coordinates
            GLfloat coords[] = {cx * size, 1 - cy * size - size,
                                cx * size + size, 1 - cy * size - size,
                                cx * size + size, 1 - cy * size,
                                cx * size, 1 - cy * size};

            glslfont->apply();

            /// set matrix
            glm::mat4x4 gui_modelview_matrix(
                fontSize / 1.1, 0, 0, 0,
                0, fontSize / 1.1, 0, 0,
                0, 0, 1, 0,
                (x / 1.1 + (i - minus) * fontSpaces / 1.1 + fontMoveX + 5) / 50 - 1,
                        1 - (y / 1.1 + fontMoveY + 2) / 50, -layer, 1
            );
            matrix_result = gui_projection_matrix * gui_modelview_matrix;

            /// render
            gui_shader->bind();
            gui_shader->uniformMatrix("u_Matrix", glm::value_ptr(matrix_result));
            gui_shader->attrib(vertices, coords);
            glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLubyte), GL_UNSIGNED_BYTE, indices);
        }
    }
    glDisable(GL_BLEND);
    gui_shader->unbind();
}

/**
 * @brief prepareLM prepare rendering of lightmaps
 * @param count is amount of lightmaps
 */
void gles20::prepareLM(int count) {

    /// set cube framebuffers
    for (int i = 0; i < 6; i++) {
        cube[i] = *(new RTT());
        cube[i].res = rttsize;
        cube[i].width = cube[i].res;
        cube[i].height = cube[i].res;
        initRTT(&cube[i]);
    }

    /// set lightmaps framebuffers
    lm = new RTT[count];
    for (int i = 0; i < count; i++) {
        lm[i] = *(new RTT());
        lm[i].res = rttsize;
        lm[i].width = lm[i].res;
        lm[i].height = lm[i].res;
        initRTT(&lm[i]);
        setRTT(&lm[i], 0);
        setRTT(&lm[i], 1);
    }
}

/**
 * @brief renderLMLight render light into lightmap
 * @param lightrenderer is shader to use
 */
void gles20::renderLMLight(shader* lightrenderer) {
    enable[1] = false;
    glm::vec4 pos = light.u_light;
    glm::vec4 dir = light.u_light_dir;

    float x = pos.x;
    float y = pos.y;
    float z = pos.z;

    /// lightmap cycle
    for (int i = 0; i < trackdata->getLMCount(); i++) {

        /// cubemap sides
        for (int v = 0; v < 6; v++) {

            /// set view
            perspective(90, 1, 0.1, 300);
            switch(v) {
                case(0):
                    lookAt(x, y, z, x, y - 1, z, 1, 0, 0);
                    break;
                case(1):
                    lookAt(x, y, z, x, y + 1, z, 1, 0, 0);
                    break;
                case(2):
                    lookAt(x, y, z, x - 1, y, z, 0, 1, 0);
                    break;
                case(3):
                    lookAt(x, y, z, x + 1, y, z, 0, 1, 0);
                    break;
                case(4):
                    lookAt(x, y, z, x, y, z - 1, 0, 1, 0);
                    break;
                case(5):
                    lookAt(x, y, z, x, y, z + 1, 0, 1, 0);
                    break;
            }

            /// render shadowmap
            if (i == 0) {
                renderShadowMap = true;
                setRTT(&cube[v], 0);
                overshader = shadowmap;
                renderModel(trackdata);
                overshader = 0;
                renderShadowMap = false;
            }

            /// update lightmap
            light.u_light = view_matrix * pos;
            light.u_light_dir = view_matrix * dir;
            lmFilter = i;
            setRTT(&lm[i], 2);
            overmode = 1;
            overshader = lightrenderer;
            glActiveTexture( GL_TEXTURE2 );
            glBindTexture(GL_TEXTURE_2D, cube[v].rendertexture2);
            glActiveTexture( GL_TEXTURE0 );
            renderModel(trackdata);
            overmode = 0;
            overshader = 0;
            lmFilter = -1;
        }
    }
}

/**
 * @brief saveLMs save lightmap into file
 */
void gles20::saveLMs() {
#ifndef ANDROID
    /// save lightmap into file
    for (int i = 0; i < trackdata->getLMCount(); i++) {

        /// get pixels
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        GLubyte* pixels = new GLubyte[rttsize * rttsize * 4];
        GLubyte* pixels2 = new GLubyte[rttsize * rttsize * 4];
        glBindTexture(GL_TEXTURE_2D, lm[i].rendertexture);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels2);

        /// fill holes
        for (int a = 0; a < rttsize; a++) {
            for (int b = 0; b < rttsize; b++) {
                if (pixels[(b * rttsize + a) * 4 + 3] < 128) {
                    for (int i = -1; i <= 1; i++) {
                        for (int j = -1; j <= 1; j++) {
                            if ((a + i >= 0) && (a + i < rttsize) && (b + j >= 0) && (b + j < rttsize)) {
                                if (pixels[((b + j) * rttsize + a + i) * 4 + 3] > 128) {
                                    for (int k = 0; k < 3; k++) {
                                        pixels[(b * rttsize + a) * 4 + k] = pixels[((b + j) * rttsize + a + i) * 4 + k];
                                    }
                                    pixels[(b * rttsize + a) * 4 + 3] = 128;
                                }
                            }
                            if (pixels[(b * rttsize + a) * 4 + 3] == 128) {
                                break;
                            }
                        }
                        if (pixels[(b * rttsize + a) * 4 + 3] == 128) {
                            break;
                        }
                    }
                }
            }
        }

        /// blur
        for (int a = 0; a < rttsize; a++) {
            for (int b = 0; b < rttsize; b++) {
                for (int k = 0; k < 3; k++) {
                    int count = 1;
                    int value = pixels[(b * rttsize + a) * 4 + k];
                    for (int i = -1; i <= 1; i++) {
                        for (int j = -1; j <= 1; j++) {
                            if ((b + j >= 0) && (b + j < rttsize))
                                if ((a + i >= 0) && (a + i < rttsize))
                                    if (pixels[((b + j) * rttsize + a + i) * 4 + 3] > 128) {
                                        count++;
                                        value += pixels2[((b + j) * rttsize + a + i) * 4 + k];
                                    }
                        }
                    }
                    pixels[(b * rttsize + a) * 4 + k] = value / count;
                }
            }
        }

        /// fix alpha channel
        for (int a = 0; a < rttsize * rttsize; a++) {
            pixels[a * 4 + 3] = 255;
        }

        /// write file
        char filename[256];
        sprintf(filename, "lightmap%d.png", i);
        writeImage(prefix(filename), rttsize, rttsize, pixels);

        /// delete arrays
        delete[] pixels;
        delete[] pixels2;
    }
#endif
}

/**
 * @brief setMode sets rendering mode
 * @param mode is mode index
 */
void gles20::setRTT(RTT *r, int mode) {

    /// prepare empty framebuffer
    if (mode == 0) {
        glBindFramebuffer(GL_FRAMEBUFFER, r->fboID);
        glViewport (0, 0, r->width, r->height);
        clear(true);
    }

    /// render texture to screen
    if (mode == 1) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport (0, 0, (GLsizei) screen_width, (GLsizei) screen_height);
        scene_shader->bind();

        /// indicies
        GLubyte indices[] = {3,0,1,3,1,2};

        /// vertices
        GLfloat vertices[] = {
            -1, -1, 0,
            +1, -1, 0,
            +1, +1, 0,
            -1, +1, 0
        };

        /// coords
        GLfloat coords[] = {0, 0,
                            r->width / (float)r->res, 0,
                            r->width / (float)r->res, r->height / (float)r->res,
                            0, r->height / (float)r->res
        };

        /// texture
        glBindTexture(GL_TEXTURE_2D, r->rendertexture);
        scene_shader->attrib(vertices, coords);
        scene_shader->uniformFloat("u_res", 1 / (float)r->res);

        /// render
        glDisable(GL_DEPTH_TEST);
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLubyte), GL_UNSIGNED_BYTE, indices);
        scene_shader->unbind();
    }

    /// prepare framebuffer
    if (mode == 2) {
        glBindFramebuffer(GL_FRAMEBUFFER, r->fboID);
        glViewport (0, 0, r->width, r->height);
        clear(false);
    }
}
