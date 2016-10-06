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

#include <glm/glm.hpp>
#include <vector>

/**
 * @brief The octreeNode class
 */
class octreeNode {
public:

    octreeNode* next[8];  ///< Childs

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
};

#endif // OCTREENODE_H
