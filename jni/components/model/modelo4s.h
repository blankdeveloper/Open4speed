//----------------------------------------------------------------------------------------
/**
 * \file       modelo4s.h
 * \author     Vonasek Lubos
 * \date       2014/01/05
 * \brief      Class for loading models into OpenGL list, it parse full model file, apply
 *             textures, materials and blending.
*/
//----------------------------------------------------------------------------------------

#ifndef MODELO4S_H
#define MODELO4S_H

#include "stdafx.h"

/**
 * @brief The model loader from o4s file
 */
class modelo4s : public model {
public:

    /**
     * @brief Constructor for loading model from file
     * @param filename is path and name of file to load
     * @param lightmaps is true to load lightmaps
     */
    modelo4s(const char* filename, bool lightmaps);

    /**
     * @brief getLMCount get amount of lightmaps
     * @return lightmaps count
     */
    int getLMCount();
};

#endif // MODELO4S_H
