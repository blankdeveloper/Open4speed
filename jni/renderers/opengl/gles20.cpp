///----------------------------------------------------------------------------------------
/**
 * \file       gles20.cpp
 * \author     Vonasek Lubos
 * \date       2014/12/30
 * \brief      GL renderer draws geometry and other things on screen
**/
///----------------------------------------------------------------------------------------

#define GLM_FORCE_RADIANS
#include <glm/gtc/type_ptr.hpp>
#include "engine/io.h"
#include "renderers/opengl/gles20.h"

#ifdef ANDROID
#define PACKED_EXTENSION "GL_OES_packed_depth_stencil"
#define PACKED_EXT GL_DEPTH24_STENCIL8_OES
#endif

/**
 * @brief gles20 constructor
 */
gles20::gles20()
{

    /// set default values
    for (int i = 0; i < 10; i++)
        enable[i] = true;
    oddFrame = true;

    fboID = 0;
    rboID = 0;
    rendertexture = 0;
    scene = 0;
    shadow = 0;
}

/**
 * @brief gles20 destructor
 */
gles20::~gles20()
{
    cleanup();
}

void gles20::cleanup()
{
    if (fboID)
    {
        glDeleteFramebuffers(2, fboID);
        delete[] fboID;
        fboID = 0;
    }
    if (rboID)
    {
        glDeleteRenderbuffers(2, rboID);
        delete[] rboID;
        rboID = 0;
    }
    if (rendertexture)
    {
        glDeleteTextures(2, rendertexture);
        delete[] rendertexture;
        rendertexture = 0;
    }
    if (scene)
    {
        delete scene;
        scene = 0;
    }
    if (shadow)
    {
        delete shadow;
        shadow = 0;
    }
}

/**
 * @brief init inits renderer
 * @param w is screen width
 * @param h is screen height
 * @param a is screen aliasing(reducing resolution)
 */
void gles20::init(int w, int h, float a)
{
    aliasing = a;
    width = w;
    height = h;
    cleanup();

    //find ideal texture resolution
    int resolution = 2;
    while (resolution < width)
        resolution *= 2;

    //create frame buffer
    fboID = new GLuint[2];
    rboID = new GLuint[2];
    rendertexture = new GLuint[2];

    //framebuffer textures
    glGenTextures(2, rendertexture);
    for (int i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_2D, rendertexture[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    }

    /// create render buffers for depth buffer and stencil buffer
    int stencilIndex = 1;
    glGenRenderbuffers(2, rboID);
    glBindRenderbuffer(GL_RENDERBUFFER, rboID[0]);
#ifdef ANDROID
    char* extString = (char*)glGetString(GL_EXTENSIONS);
    if (strstr(extString, PACKED_EXTENSION) != 0)
    {
        glRenderbufferStorage(GL_RENDERBUFFER, PACKED_EXT, width, height);
        stencilIndex = 0;
    }
    else
#endif
    {
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, rboID[1]);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, width, height);
    }

    //framebuffers
    glGenFramebuffers(2, fboID);
    for (int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fboID[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rendertexture[i], 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboID[0]);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboID[stencilIndex]);
    }

    /// check FBO status
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
        rttComplete = true;
    else
    {
        char error[128];
        sprintf(error, "%d", glCheckFramebufferStatus(GL_FRAMEBUFFER));
        logi("glCheckFramebufferStatus", error);
        rttComplete = false;
    }

    //set viewport
    glViewport (0, 0, (GLsizei) width, (GLsizei) height);
    glClear(GL_COLOR_BUFFER_BIT);
    glActiveTexture( GL_TEXTURE0 );

    //set shaders
    scene = new glsl(getList("VERT", "#assets/shaders/scene.glsl"), getList("FRAG", "#assets/shaders/scene.glsl"));
    shadow = new glsl(getList("VERT", "#assets/shaders/shadow.glsl"), getList("FRAG", "#assets/shaders/shadow.glsl"));
}

/**
 * @brief renderDynamic render dynamic objects
 * @param geom is geometry vbo
 * @param sh is shader to use
 * @param txt is texture to use
 * @param triangleCount is triangle count
 */
void gles20::renderDynamic(float* vertices, float* normals, float* coords, shader* sh, texture* txt, int triangleCount)
{
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
    /*if (!m->v3d.empty())
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
    }*/

    /// set opengl for rendering models
    for (unsigned int i = 0; i < m->models.size(); i++)
        if (enable[m->models[i].filter] && !m->models[i].touchable)
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
void gles20::renderShadow(model* m)
{
    if (!rttComplete)
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
    for (int pass = 1; pass <= 1; pass++)
    {
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
void gles20::renderSubModel(model3d *m)
{
    /// set model matrix
    glm::mat4x4 modelView;
    if (m->dynamic)
    {
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
            m->dynamicMat[0], m->dynamicMat[1], m->dynamicMat[2], m->dynamicMat[3],
            m->dynamicMat[4], m->dynamicMat[5], m->dynamicMat[6], m->dynamicMat[7],
            m->dynamicMat[8], m->dynamicMat[9], m->dynamicMat[10], m->dynamicMat[11],
            m->dynamicMat[12], m->dynamicMat[13], m->dynamicMat[14], m->dynamicMat[15]
        );
        modelMat = dynamic * translation;
        modelView = view_matrix * modelMat;
    } else
    {
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
    glBindTexture(GL_TEXTURE_2D, rendertexture[oddFrame]);
    current->uniformInt("EnvMap1", 1);

    /// set texture
    glActiveTexture( GL_TEXTURE0 );
    m->texture2D->apply();
    current->uniformInt("color_texture", 0);

    /// set uniforms
    current->uniformFloat("u_width", 1 / (float)width / aliasing);
    current->uniformFloat("u_height", 1 / (float)height / aliasing);
    if (enable[9])
        current->uniformFloat("u_brake", 1.0f);
    else
        current->uniformFloat("u_brake", 0.0f);
    current->attrib(&m->vertices[0], &m->normals[0], &m->coords[0]);
    glDrawArrays(GL_TRIANGLES, 0, m->vertices.size() / 3);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/**
 * @brief rtt enables rendering into FBO which makes posible to do reflections
 * @param enable is true to start drawing, false to render on screen
 */
void gles20::rtt(bool enable)
{
    if (enable)
    {
#ifndef ANDROID
        /// start timer
        glGenQueries(1,gpuMeasuring);
        glBeginQuery(GL_TIME_ELAPSED, gpuMeasuring[0]);
#endif
        glBindFramebuffer(GL_FRAMEBUFFER, fboID[oddFrame]);
        glViewport (0, 0, width * aliasing, height * aliasing);
        glClearStencil(0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(true);
        oddFrame = !oddFrame;
    } else
    {
#ifndef ANDROID
        glEndQuery(GL_TIME_ELAPSED);

        /// get scene counters
        GLint gpu_time = 0;
        glGetQueryObjectiv(gpuMeasuring[0], GL_QUERY_RESULT, &gpu_time);

        GLint copy_time = 0;
        glBeginQuery(GL_TIME_ELAPSED, gpuMeasuring[0]);
#endif
        /// prepare rendering
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport (0, 0, width, height);

        /// vertices
        float vertices[] =
        {
            -1, +1, 0,
            -1, -1, 0,
            +1, -1, 0,
            -1, +1, 0,
            +1, -1, 0,
            +1, +1, 0,
        };

        /// coords
        float coords[] =
        {
            0, 1,
            0, 0,
            1, 0,
            0, 1,
            1, 0,
            1, 1,
        };

        scene->bind();
        glBindTexture(GL_TEXTURE_2D, rendertexture[oddFrame]);
        glDisable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glDepthMask(false);
        /// render
        scene->attrib(vertices, 0, coords);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDepthMask(true);

#ifndef ANDROID
        glEndQuery(GL_TIME_ELAPSED);
        glGetQueryObjectiv(gpuMeasuring[0], GL_QUERY_RESULT, &copy_time);
        printf("3D time: %dk 2D time: %dk\n", gpu_time / 1000, copy_time / 1000);
#endif
    }
}
