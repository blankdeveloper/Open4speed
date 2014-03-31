//----------------------------------------------------------------------------------------
/**
 * \file       octreeNode.h
 * \author     Vonasek Lubos
 * \date       2014/02/22
 * \brief      Class for render optimalization using octree
*/
//----------------------------------------------------------------------------------------

#include <glm/glm.hpp>
#include <vector>
#include "interfaces/model.h"
#include "interfaces/renderer.h"
#include "raycaster/triangle.h"

#ifndef OCTREENODE_H
#define OCTREENODE_H

/**
 * @brief The octreeNode class
 */
class octreenode {
public:

    int depth;                      ///< Depth of node
    bool childCount;                ///< Information about child count
    bool hasNext[8];                ///< Information about childs
    octreenode *next[8];            ///< Childs
    AABB *reg;                      ///< AABB region
    std::vector<triangle*> list;    ///< To render list

    /**
     * @brief octreeNode is a constructor of root node
     * @param r is node region
     * @param data is a vector of triangles to insert
     */
    octreenode(AABB *r, std::vector<triangle*> geom);

    /**
     * @brief octreeNode is constructor for subnodes
     * @param r is subnode region
     * @param depth is depth of node
     */
    octreenode(AABB *r, int depth);

    void addTriangleToDebug();

    /**
     * @brief createSubNodes creates subnodes of current node
     */
    void createSubNodes();

    void debug(bool test);

    /**
     * @brief getSubregion gets subregion
     * @param x is position coordinate
     * @param y is position coordinate
     * @param z is position coordinate
     * @return instance of region structure
     */
    AABB* getSubregion(bool x, bool y, bool z);

    bool isIntersected(glm::vec3 begin, glm::vec3 end, int ignore1, int ignore2);

    bool isIntersectedEx(glm::vec3 begin, glm::vec3 end, int ignore1, int ignore2);
};

#endif // OCTREENODE_H
