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
    bool update;                    ///< Update state
    float distance;                 ///< Distance for TR Coherency
    int lastUpdate;                 ///< Last update ID
    glm::mat4x4 previous_matrix;    ///< Previous frustum matrix
    glm::vec4 position[8];          ///< Cached positions of AABB in frustum
    octreeNode *next[8];            ///< Childs
    region *reg;                    ///< AABB region
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
    octreeNode(region *r);

    /**
     * @brief basicTest tests AABB with frustm planes
     * @return 0 if it is outside, 1 if it is partitionaly inside, 2 if it is inside
     */
    int basicTest();

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
    region* getSubregion(bool x, bool y, bool z);

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

    /**
     * @brief render renders current node
     * @param renderer is instance of renderer
     * @param gamma is requested amount of gamma
     */
    void render(renderer* renderer, float gamma);
};

#endif // OCTREENODE_H
