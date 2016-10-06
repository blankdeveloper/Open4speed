//----------------------------------------------------------------------------------------
/**
 * \file       octreeNode.h
 * \author     Vonasek Lubos
 * \date       2013/04/20
 * \brief      Class for render optimalization using octree
*/
//----------------------------------------------------------------------------------------

#ifndef OCTREENODE_H
#define OCTREENODE_H

#include "engine/math.h"
#include "interfaces/texture.h"
#include <glm/glm.hpp>
#include <vector>

/**
 * @brief The octreeNode class
 */
class octreeNode {
public:

    /**
     * @brief addVoxel adds voxel into tree
     * @param x is position x coordinate
     * @param y is position y coordinate
     * @param z is position z coordinate
     * @param c is voxel color
     */
    void addVoxel(float x, float y, float z, ColorRGBA c);

    /**
     * @brief getTransform gets position of AABB vertex in frustum space
     * @param i is index of vertex
     * @return transformed vertex in frustum space
     */
    glm::vec4 getTransform(int i);

    /**
     * @brief process processes node and subnodes
     */
    void process();

    /**
     * @brief processAll adds all into to render list
     */
    void processAll();

    std::pair<std::vector<float>, std::vector<float> > getCulledVoxels(glm::mat4x4 matrix, float far, AABB aabb);
private:
    octreeNode* next[8];  ///< Childs
    //TODO:remove
    std::vector<float> voxelCoord;
    std::vector<float> voxelColor;
};

#endif // OCTREENODE_H
