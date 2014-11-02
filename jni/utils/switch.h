//----------------------------------------------------------------------------------------
/**
 * \file       switch.h
 * \author     Vonasek Lubos
 * \date       2014/11/01
 * \brief      This utility switches between components. Switch depends on configuration
 *             file.
*/
//----------------------------------------------------------------------------------------

#ifndef SWITCH_H
#define SWITCH_H

#include <string>
#include "interfaces/input.h"
#include "interfaces/model.h"
#include "interfaces/physics.h"
#include "interfaces/renderer.h"
#include "interfaces/shader.h"
#include "interfaces/texture.h"
#include "interfaces/vbo.h"

void clearMediaStorage();

/**
 * @brief getInput gets input controller
 * @return input controller
 */
input* getInput();

/**
 * @brief getModel gets model
 * @param filename is path and name of file to load
 * @return instance of model
 */
model* getModel(std::string filename);

/**
 * @brief getPhysics gets physical engine
 * @param m is 3D model of scene
 * @return physical engine
 */
physics* getPhysics(model *m);

/**
 * @brief getRenderer gets renderer
 * @return renderer instance
 */
renderer* getRenderer(int w, int h);

/**
 * @brief getShader compile shader from file
 * @param name is shader filename
 * @return instance of shader
 */
shader* getShader(std::string name);

/**
 * @brief getTexture gets texture
 * @param filename is filename of texture
 * @param alpha is amount of blending
 * @return texture instance
 */
texture* getTexture(std::string filename, float alpha);

/**
 * @brief getTexture gets texture
 * @param r is amount of red from 0 to 1
 * @param g is amount of green from 0 to 1
 * @param b is amount of blue from 0 to 1
 * @param alpha is amount of blending
 * @return texture instance
 */
texture* getTexture(float r, float g, float b, float alpha);

/**
 * @brief glvbo creates VBO from data
 * @param size is amount of vertices
 * @param vertices is vertices array
 * @param normals is normals array
 * @param coords is texture coords array
 * @param tnormals is triangle normals array
 */
vbo* getVBO(int size, float* vertices, float* normals, float* coords, float* tnormals, bool dynamic);

#endif // SWITCH_H
