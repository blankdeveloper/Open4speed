//----------------------------------------------------------------------------------------
/**
 * \file       dynamiclight.cpp
 * \author     Vonasek Lubos
 * \date       2014/02/14
 * \brief      Class for loading dynamic light data for lightmaps
*/
//----------------------------------------------------------------------------------------

#include "interfaces/vbo.h"
#include "loaders/dynamiclight.h"
#include "renderers/opengl/gles20.h"
#include "utils/io.h"
#include "utils/switch.h"
#include "common.h"

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
    fboRenderer = getShader("lmPoints");

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
        lightVBO.push_back(getVBO(sizeof(float)*size, vertices, 0, 0, 0));
        delete[] vertices;
    }

#ifdef ZIP_ARCHIVE
    zip_fclose(file);
#else
    fclose(file);
#endif
}


/**
 * @brief setLight update lightmaps by loaded data
 * @param index is index of the light
 * @param value is true for light on, false for light off
 */
void DynamicLight::setLight(int index, bool value) {
    int pIndex = index * lmCount;
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    if (value)
        glBlendEquation(GL_FUNC_ADD);
    else
        glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);

    for (int i = 0; i < lmCount; i++) {
        //one light
        if (index >= 0) {
            if ((lightParam[pIndex + i]->len > 0) && (lightParam[pIndex + i]->enabled != value)) {
                trackdata->lightmaps[i]->bindFBO();
                fboRenderer->uniformFloat4("color", lightParam[pIndex + i]->r, lightParam[pIndex + i]->g, lightParam[pIndex + i]->b, 0);
                lightVBO[i]->render(fboRenderer, lightParam[pIndex + i]->begin / 3, lightParam[pIndex + i]->len / 3);
                trackdata->lightmaps[i]->unbindFBO();
                lightParam[pIndex + i]->enabled = value;
            }
        }
        //all lights
        else {
            int lightCount = lightParam.size() / lmCount;
            int last = (lightCount - 1) * lmCount + i;
            if (lightParam[last]->enabled != value) {
                int len = lightParam[last]->begin / 3 + lightParam[last]->len / 3;
                trackdata->lightmaps[i]->bindFBO();
                fboRenderer->uniformFloat4("color", lightParam[last]->r, lightParam[last]->g, lightParam[last]->b, 0);
                lightVBO[i]->render(fboRenderer, 0, len);
                trackdata->lightmaps[i]->unbindFBO();
                for (int j = 0; j < lightCount; j++) {
                    lightParam[j * lmCount + i]->enabled = value;
                }
            }
        }
    }
    glDisable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
}
