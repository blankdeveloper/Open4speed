///----------------------------------------------------------------------------------------
/**
 * \file       renderer.h
 * \author     Vonasek Lubos
 * \date       2014/12/30
 * \brief      Renderer draws geometry and other things on screen
**/
///----------------------------------------------------------------------------------------

#ifndef RENDERER_H
#define RENDERER_H

#include "engine/matrices.h"
#include "engine/model.h"
#include "interfaces/shader.h"
#include "interfaces/texture.h"

/**
 * @brief The renderer interface
 */
class renderer : public matrices
{
public:

    float aliasing;      ///< Screen detail
    bool enable[10];     ///< Enabled filter
    int width;           ///< Screen width
    int height;          ///< Screen height

    /**
     * @brief renderer destructor
     */
    virtual ~renderer() {}

    /**
     * @brief init inits renderer
     * @param w is screen width
     * @param h is screen height
     * @param a is screen aliasing(reducing resolution)
     */
    virtual void init(int w, int h, float a) = 0;

    /**
     * @brief renderDynamic render dynamic objects
     * @param geom is geometry vbo
     * @param sh is shader to use
     * @param txt is texture to use
     * @param triangleCount is triangle count
     */
    virtual void renderDynamic(float* vertices, float* normals, float* coords, shader* sh, texture* txt, int triangleCount) = 0;

    /**
     * @brief renderModel renders model into scene
     * @param m is instance of model to render
     */
    virtual void renderModel(model* m) = 0;

    /**
     * @brief renderShadow renders shadow of model into scene
     * @param m is instance of model to render
     */
    virtual void renderShadow(model* m) = 0;

    /**
     * @brief renderSubModel renders model into scene
     * @param m is instance of model to render
     */
    virtual void renderSubModel(model3d *m) = 0;

    /**
     * @brief rtt enables rendering into FBO which makes posible to do reflections
     * @param enable is true to start drawing, false to render on screen
     */
    virtual void rtt(bool enable) = 0;
};

#endif // RENDERER_H
