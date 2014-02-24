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

#ifndef OCTREENODE_H
#define OCTREENODE_H

/**
 * @brief The data struct
 */
struct data {
    model3d* model; ///< model instance
    int updateId;   ///< last update id
};

/**
 * @brief The octreeNode class
 */
class octreeNode {
public:

    bool hasNext[8];                ///< Information about childs
    octreeNode *next[8];            ///< Childs
    AABB *reg;                    ///< AABB region
    std::vector<data> list;         ///< To render list

    /**
     * @brief octreeNode is a constructor of root node
     * @param size is size of a world
     * @param m is model to add
     */
    octreeNode(float size, model *m);

    /**
     * @brief octreeNode is constructor for subnodes
     * @param r is subnode region
     */
    octreeNode(AABB *r);

    /**
     * @brief createSubNodes creates subnodes of current node
     */
    void createSubNodes();

    /**
     * @brief getSubregion gets subregion
     * @param x is position coordinate
     * @param y is position coordinate
     * @param z is position coordinate
     * @return instance of region structure
     */
    AABB* getSubregion(bool x, bool y, bool z);
};

#endif // OCTREENODE_H
