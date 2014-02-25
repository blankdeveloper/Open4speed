//----------------------------------------------------------------------------------------
/**
 * \file       octreeNode.cpp
 * \author     Vonasek Lubos
 * \date       2014/02/22
 * \brief      Class for render optimalization using octree
*/
//----------------------------------------------------------------------------------------

#include "raycaster/octreenode.h"
#include "common.h"

std::vector<int> octreeStatus;

/**
 * @brief octreeNode is constructor for subnodes
 * @param r is subnode region
 * @param depth is depth of node
 */
octreenode::octreenode(AABB *r, int depth) {
    reg = r;
    this->depth = depth;
    for (int i = 0; i < 8; i++) {
        hasNext[i] = false;
    }
}

/**
 * @brief octreeNode is a constructor of root node
 * @param size is size of a world
 * @param data is a vector of triangles to insert
 */
octreenode::octreenode(float size, std::vector<triangle*> geom) {
    /// Set limits of tree
    depth = 0;
    reg = new AABB();
    reg->min.x = -size / 2;
    reg->min.y = -size / 2;
    reg->min.z = -size / 2;
    reg->max.x = size / 2;
    reg->max.y = size / 2;
    reg->max.z = size / 2;
    reg->size = size;

    for (int i = 0; i < 8; i++) {
        hasNext[i] = false;
    }

    /// Add top models
    octreeStatus.push_back(geom.size());
    for (unsigned int i = 0; i < geom.size(); i++) {
        geom[i]->fixed = false;
        list.push_back(geom[i]);
    }

    createSubNodes();
}

void octreenode::addTriangleToDebug() {
    if (depth + 1 == octreeStatus.size()) {
        octreeStatus.push_back(1);
    } else {
        octreeStatus[depth + 1]++;
    }
}

/**
 * @brief createSubNodes creates subnodes of current node
 */
void octreenode::createSubNodes() {
    AABB* r[8];
    r[0] = getSubregion(0, 0, 0);
    r[1] = getSubregion(0, 0, 1);
    r[2] = getSubregion(0, 1, 0);
    r[3] = getSubregion(0, 1, 1);
    r[4] = getSubregion(1, 0, 0);
    r[5] = getSubregion(1, 0, 1);
    r[6] = getSubregion(1, 1, 0);
    r[7] = getSubregion(1, 1, 1);

    for (unsigned int i = 0; i < list.size(); i++) {
        if (!list[i]->fixed) {
            bool moved = false;
            for (int j = 0; j < 8; j++) {
                bool ok = true;
                if (r[j]->size < list[i]->reg.size / 2.0f) {
                    ok = false;
                }
                if ((list[i]->reg.max.x < r[j]->min.x) || (r[j]->max.x < list[i]->reg.min.x)) {
                    ok = false;
                }
                if ((list[i]->reg.max.y < r[j]->min.y) || (r[j]->max.y < list[i]->reg.min.y)) {
                    ok = false;
                }
                if ((list[i]->reg.max.z < r[j]->min.z) || (r[j]->max.z < list[i]->reg.min.z)) {
                    ok = false;
                }

                if (ok) {
                    if (!hasNext[j]) {
                        next[j] = new octreenode(r[j], depth + 1);
                        hasNext[j] = true;
                    }
                    next[j]->list.push_back(list[i]);
                    addTriangleToDebug();
                    moved = true;
                }

            }
            if (!moved) {
                list[i]->fixed = true;
            } else {
                octreeStatus[depth]--;
                list.erase(list.begin() + i);
                i--;
                moved = true;
            }
        }
    }
    for (int j = 0; j < 8; j++) {
        if (hasNext[j]) {
            next[j]->createSubNodes();
        } else {
            delete r[j];
        }
    }
}

void octreenode::debug() {
    for (unsigned int i = 0; i < octreeStatus.size(); i++) {
        printf("%d triangles in depth %d\n", octreeStatus[i], i);
    }
}

/**
 * @brief getSubregion gets subregion
 * @param x is position coordinate
 * @param y is position coordinate
 * @param z is position coordinate
 * @return instance of region structure
 */
AABB* octreenode::getSubregion(bool x, bool y, bool z) {
    AABB *r = new AABB;
    if (x) {
        r->min.x = reg->min.x + reg->size / 2;
        r->max.x = reg->max.x;
    } else {
        r->min.x = reg->min.x;
        r->max.x = reg->max.x - reg->size / 2;
    }
    if (y) {
        r->min.y = reg->min.y + reg->size / 2;
        r->max.y = reg->max.y;
    } else {
        r->min.y = reg->min.y;
        r->max.y = reg->max.y - reg->size / 2;
    }
    if (z) {
        r->min.z = reg->min.z + reg->size / 2;
        r->max.z = reg->max.z;
    } else {
        r->min.z = reg->min.z;
        r->max.z = reg->max.z - reg->size / 2;
    }
    r->size = reg->size / 2;
    return r;
}
