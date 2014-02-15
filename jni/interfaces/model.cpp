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
        models[i].vboData->destroy();
        delete[] models[i].colora;
        delete[] models[i].colord;
        delete[] models[i].colors;
        delete models[i].reg;
        delete[] models[i].vertices;
        delete[] models[i].normals;
        delete[] models[i].coords;
        delete[] models[i].tid;
        delete[] models[i].triangleCount;
        models[i].texture2D->pointerDecrease();
    }
    for (unsigned int i = 0; i < lightmaps.size(); i++) {
        lightmaps[i]->destroy();
    }
    lightmaps.clear();
    models.clear();
    //delete dynamicLight;
}
