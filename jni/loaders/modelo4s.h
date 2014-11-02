//----------------------------------------------------------------------------------------
/**
 * \file       modelo4s.h
 * \author     Vonasek Lubos
 * \date       2014/11/01
 * \brief      Class for loading models into OpenGL list, it parse full model file, apply
 *             textures, materials and blending.
*/
//----------------------------------------------------------------------------------------

#ifndef MODELO4S_H
#define MODELO4S_H

#include <string>
#include "interfaces/model.h"

/**
 * @brief The model loader from o4s file
 */
class modelo4s : public model {
public:

    /**
     * @brief Constructor for loading model from file
     * @param filename is path and name of file to load
     */
    modelo4s(std::string filename);
};

#endif // MODELO4S_H
