//----------------------------------------------------------------------------------------
/**
 * \file       renderer.h
 * \author     Vonasek Lubos
 * \date       2014/01/05
 * \brief      Renderer draws geometry and other things on screen
*/
//----------------------------------------------------------------------------------------

#ifndef RENDERER_H
#define RENDERER_H

#include "stdafx.h"

/**
 * @brief The RTT struct
 */
struct RTT {
    GLuint rendertexture;        ///< Render to texture texture
    GLuint rendertexture2;       ///< Render to texture texture2
    GLuint rboID;                ///< Render buffer object id
    GLuint fboID;                ///< Frame buffer object id
    GLuint width;                ///< Image width
    GLuint height;               ///< Image height
    GLuint res;                  ///< Resolution of texture
};

/**
 * @brief The Light struct
 */
struct Light {
    glm::vec4 u_light_diffuse;  ///< Diffuse light
    float u_light_cut;          ///< Light cutoff
    float u_light_spot_eff;     ///< Light spot effect
    float u_near;               ///< Near clipping
    glm::vec4 u_light;          ///< Light position
    glm::vec4 u_light_att;      ///< Light attenuation
    glm::vec4 u_light_dir;      ///< Light direction
    glm::vec4 u_nearest1;       ///< Nearest light position
};

/**
 * @brief The renderer interface
 */
class renderer {
public:

    bool enable[10];            ///< Enabled filter
    int frame;                  ///< Frame index
    bool oddFrame;              ///< Odd frame info
    Light light;                ///< One light information(for lightmap rendering)
    RTT rtt[2];                 ///< Screen-space framebuffer
    int overmode;               ///< Special mode

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
    virtual void lookAt(GLfloat eyex, GLfloat eyey, GLfloat eyez,
                        GLfloat centerx, GLfloat centery, GLfloat centerz,
                        GLfloat upx, GLfloat upy, GLfloat upz) = 0;

    /**
     * @brief lookAt implements GLUlookAt without up vector
     * @param eyex is eye vector coordinate
     * @param eyey is eye vector coordinate
     * @param eyez is eye vector coordinate
     * @param centerx is camera center coordinate
     * @param centery is camera center coordinate
     * @param centerz is camera center coordinate
     */
    virtual void lookAt(GLfloat eyex, GLfloat eyey, GLfloat eyez,
                        GLfloat centerx, GLfloat centery, GLfloat centerz) = 0;

    /**
     * @brief perspective implements GLUPerspective
     * @param fovy is fov angle
     * @param aspect is screen aspect ration
     * @param zNear is near cutting plate
     * @param zFar is far cutting plane
     */
    virtual void perspective(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar) = 0;

    /**
     * @brief clear clears fragmet/depth buffer
     * @param colors true to clear both, false to clear only depth buffer
     */
    virtual void clear(bool colors) = 0;

    /**
     * @brief multMatrix multiplies with matrix
     * @param matrix is 4x4 matrix in OpenGL format
     */
    virtual void multMatrix(float* matrix) = 0;

    /**
     * @brief popMatrix pops matrix from stack
     */
    virtual void popMatrix() = 0;

    /**
     * @brief pushMatrix pushs current matrix to stack
     */
    virtual void pushMatrix() = 0;

    /**
     * @brief rotateX rotate around X axis
     * @param value is angle
     */
    virtual void rotateX(float value) = 0;

    /**
     * @brief rotateX rotate around Y axis
     * @param value is angle
     */
    virtual void rotateY(float value) = 0;

    /**
     * @brief rotateX rotate around Z axis
     * @param value is angle
     */
    virtual void rotateZ(float value) = 0;

    /**
     * @brief scale scales current matrix
     * @param value is amount of scale(1 to keep current)
     */
    virtual void scale(float value) = 0;

    /**
     * @brief translate translates object
     * @param x is translate coordinate
     * @param y is translate coordinate
     * @param z is translate coordinate
     */
    virtual void translate(float x, float y, float z) = 0;

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
    virtual void renderButton(float x, float y, float w, float h, float layer, texture* button, const char* text) = 0;

    virtual void renderDynamic(GLfloat *vertices, GLfloat *coords, shader* sh, texture* txt, int triangleCount) = 0;

    /**
     * @brief renderImage renders image in GUI mode
     * @param x is position x
     * @param y is position y
     * @param w is width
     * @param h is height
     * @param layer is distance from camera
     * @param image is image texture
     */
    virtual void renderImage(float x, float y, float w, float h, float layer, texture* image) = 0;

    /**
     * @brief renderModel renders model into scene
     * @param m is instance of model to render
     * @param physic is physical model instance
     * @param gamma is requested render gamma
     */
    virtual void renderModel(model* m) = 0;

    /**
     * @brief prepareLM prepare rendering of lightmaps
     * @param count is amount of lightmaps
     */
    virtual void prepareLM(int count) = 0;

    /**
     * @brief renderLMLight render light into lightmap
     * @param lightrenderer is shader to use
     */
    virtual void renderLMLight(shader* lightrenderer) = 0;

    /**
     * @brief renderLMLight render light into lightmap
     * @param lightrenderer is shader to use
     */
    virtual void saveLMs() = 0;

    /**
     * @brief renderSubModel renders model into scene
     * @param m is instance of model to render
     * @param physic is physical model instance
     * @param gamma is requested render gamma
     */
    virtual void renderSubModel(model* mod, model3d* m) = 0;

    /**
     * @brief renderText renders text in GUI mode
     * @param x is position x
     * @param y is position y
     * @param layer is distance from camera
     * @param text is button text
     */
    virtual void renderText(float x, float y, float layer, const char* text) = 0;

    /**
     * @brief setMode sets rendering mode
     * @param mode is mode index
     */
    virtual void setRTT(RTT *r, int mode) = 0;
};

#endif // RENDERER_H
