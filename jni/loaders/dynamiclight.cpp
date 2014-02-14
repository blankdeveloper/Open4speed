//----------------------------------------------------------------------------------------
/**
 * \file       dynamiclight.cpp
 * \author     Vonasek Lubos
 * \date       2014/02/14
 * \brief      Class for loading dynamic light data for lightmaps
*/
//----------------------------------------------------------------------------------------

#include "stdafx.h"

/**
 * @brief DynamicLight is a destructor
 */
DynamicLight::~DynamicLight() {
    for (unsigned int i = 0; i < lightParam.size(); i++) {
        delete lightParam[i];
    }
    for (unsigned int i = 0; i < lightVBO.size(); i++) {
        delete lightVBO[i];
    }
    lightParam.clear();
    lightVBO.clear();
}

/**
 * @brief DynamicLight is dynamic light loader
 * @param filename is path to file with dynamic light data
 */
DynamicLight::DynamicLight(char* filename) {
    /// open file
#ifdef ZIP_ARCHIVE
    zip_file* file = zip_fopen(APKArchive, prefix(filename), 0);
#else
    FILE* file = fopen(prefix(filename), "r");
#endif
    char* line = new char[1024];

    /// get data size
    gets(line, file);
    lmCount = scandec(line);
    gets(line, file);
    lightCount = scandec(line);

    /// get info about lights
    for (int i = 0; i < lightCount; i++)
        for (int j = 0; j < lmCount; j++) {
            VBOLightParam* lp = new VBOLightParam();
            gets(line, file);
            sscanf(line, "%d %d %f %f %f", &lp->begin, &lp->len, &lp->r, &lp->g, &lp->b);
            lp->enabled = false;
            lightParam.push_back(lp);
        }

    ///get VBO data
    for (int i = 0; i < lmCount; i++) {
        gets(line, file);
        int size = scandec(line);
        float* vertices = new float[size * 3];
        for (int j = 0; j < size; j++) {
            gets(line, file);
            sscanf(line, "%f %f %f", &vertices[j * 3 + 0], &vertices[j * 3 + 1], &vertices[j * 3 + 2]);
        }
        lightVBO.push_back(getVBO(size, vertices, 0, 0, 0));
        delete[] vertices;
    }

#ifdef ZIP_ARCHIVE
    zip_fclose(file);
#else
    fclose(file);
#endif
}
