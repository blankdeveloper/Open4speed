//----------------------------------------------------------------------------------------
/**
 * \file       model.h
 * \author     Vonasek Lubos
 * \date       2014/02/11
 * \brief      Loading and storing models
*/
//----------------------------------------------------------------------------------------

#include "interfaces/model.h"

/**
 * @brief model destructor
 */
model::~model() {
    for (unsigned int i = 0; i < models.size(); i++) {
        if (models[i].vboData != 0)
          delete models[i].vboData;
        delete[] models[i].vertices;
        delete[] models[i].normals;
        delete[] models[i].tnormals;
        delete[] models[i].coords;
        delete[] models[i].triangleCount;
    }
    models.clear();
}
