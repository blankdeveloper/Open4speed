//----------------------------------------------------------------------------------------
/**
 * \file       model.h
 * \author     Vonasek Lubos
 * \date       2014/01/05
 * \brief      Loading and storing models
*/
//----------------------------------------------------------------------------------------

#include "stdafx.h"

/**
 * @brief model destructor
 */
model::~model() {
    for (unsigned int i = 0; i < models.size(); i++) {
        glDeleteBuffers(1, &models[i].vbo);
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
        if (models[i].usingLightmaps) {
            //models[i].lightmap->pointerDecrease();
        }
    }
    models.clear();
}
