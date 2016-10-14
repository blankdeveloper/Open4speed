///----------------------------------------------------------------------------------------
/**
 * \file       materialLoader.h
 * \author     Vonasek Lubos
 * \date       2016/10/14
 * \brief      Interface for loading materials
**/
///----------------------------------------------------------------------------------------

#ifndef MATERIALLOADER_H
#define MATERIALLOADER_H

#include <string.h>
#include "interfaces/shader.h"
#include "interfaces/texture.h"

class materialLoader
{
public:
    /**
     * @brief getShader compile shader from file
     * @param name is shader filename
     * @return instance of shader
     */
    virtual shader* getShader(std::string name) = 0;

    /**
     * @brief getTexture gets texture
     * @param filename is filename of texture
     * @return texture instance
     */
    virtual texture* getTexture(std::string filename) = 0;

    /**
     * @brief getTexture gets texture
     * @param r is amount of red from 0 to 1
     * @param g is amount of green from 0 to 1
     * @param b is amount of blue from 0 to 1
     * @return texture instance
     */
    virtual texture* getTexture(float r, float g, float b) = 0;
};

#endif // MATERIALLOADER_H
