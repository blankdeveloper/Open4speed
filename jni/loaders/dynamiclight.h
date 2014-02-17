//----------------------------------------------------------------------------------------
/**
 * \file       dynamiclight.cpp
 * \author     Vonasek Lubos
 * \date       2014/02/14
 * \brief      Class for loading dynamic light data for lightmaps
*/
//----------------------------------------------------------------------------------------

#ifndef DYNAMICLIGHT_H
#define DYNAMICLIGHT_H

#include <vector>
#include "interfaces/vbo.h"

/**
 * @brief The VBOLightParam struct is struct to store light color and vbo pointers
 */
struct VBOLightParam {
    int begin;
    int len;
    float r;
    float g;
    float b;
    bool enabled;
};

/**
 * @brief Class for loading dynamic light data for lightmaps
 */
class DynamicLight
{
public:

    int lmCount;
    int lightCount;
    shader* fboRenderer;
    std::vector<VBOLightParam*> lightParam;
    std::vector<vbo*> lightVBO;

    /**
     * @brief DynamicLight is a destructor
     */
    ~DynamicLight();

    /**
     * @brief DynamicLight is dynamic light loader
     * @param filename is path to file with dynamic light data
     */
    DynamicLight(char* filename);

    /**
     * @brief setLight update lightmaps by loaded data
     * @param index is index of the light
     * @param value is true for light on, false for light off
     */
    void setLight(int index, bool value);
};

#endif // DYNAMICLIGHT_H
