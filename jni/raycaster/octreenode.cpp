//----------------------------------------------------------------------------------------
/**
 * \file       octreeNode.cpp
 * \author     Vonasek Lubos
 * \date       2014/02/22
 * \brief      Class for render optimalization using octree
*/
//----------------------------------------------------------------------------------------

#include "raycaster/octreenode.h"
#include "raycaster/utils.h"
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
 * @param r is node region
 * @param data is a vector of triangles to insert
 */
octreenode::octreenode(AABB *r, std::vector<triangle*> geom) {
    /// Set limits of tree
    depth = 0;
    reg = r;

    for (int i = 0; i < 8; i++) {
        hasNext[i] = false;
    }
    childCount = 0;

    /// Add top models
    octreeStatus.push_back(geom.size());
    for (unsigned int i = 0; i < geom.size(); i++) {
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

    int size = list.size();
    if (size > 20) {
        for (unsigned int i = 0; i < list.size(); i++) {
            bool moved = false;
            for (int j = 0; j < 8; j++) {
                if (TestAABBAABB(&list[i]->reg, r[j])) {
                    if (!hasNext[j]) {
                        next[j] = new octreenode(r[j], depth + 1);
                        hasNext[j] = true;
                    }
                    next[j]->list.push_back(list[i]);
                    addTriangleToDebug();
                    moved = true;
                }

            }
            if (moved) {
                octreeStatus[depth]--;
                list.erase(list.begin() + i);
                i--;
                moved = true;
            }
        }
    }

    bool subdivide = false;
    for (int j = 0; j < 8; j++) {
        if (hasNext[j]) {
            next[j]->createSubNodes();
            subdivide = true;
            childCount++;
        } else {
            delete r[j];
        }
    }

    if (!subdivide && (list.size() > 50)) {
        printf("Warning: node with depth %d has %d triangles\n", depth, list.size());
    }
}

long lastTestID = -1;
long AABBTests = 0;
long triangleTests = 0;

void octreenode::debug(bool tests) {
    if (tests) {
        printf("%d AABBs tested, %d triangles tested\n", AABBTests, triangleTests);
    } else {
        for (unsigned int i = 0; i < octreeStatus.size(); i++) {
            printf("%d triangles in depth %d\n", octreeStatus[i], i);
        }
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
        r->min.x = reg->min.x + (reg->max.x - reg->min.x) / 2;
        r->max.x = reg->max.x;
    } else {
        r->min.x = reg->min.x;
        r->max.x = reg->max.x - (reg->max.x - reg->min.x) / 2;
    }
    if (y) {
        r->min.y = reg->min.y + (reg->max.y - reg->min.y) / 2;
        r->max.y = reg->max.y;
    } else {
        r->min.y = reg->min.y;
        r->max.y = reg->max.y - (reg->max.y - reg->min.y) / 2;
    }
    if (z) {
        r->min.z = reg->min.z + (reg->max.z - reg->min.z) / 2;
        r->max.z = reg->max.z;
    } else {
        r->min.z = reg->min.z;
        r->max.z = reg->max.z - (reg->max.z - reg->min.z) / 2;
    }
    return r;
}

bool octreenode::isIntersected() {

    if (utestID != lastTestID) {
        lastTestID = utestID;
        AABBTests = 0;
        triangleTests = 0;
        if (lastIntersectedTriangle != 0) {
            triangleTests++;
            if (lastIntersectedTriangle->isIntersectedByRay())
                return true;
        }
    }

    /// current node test
    for (unsigned int i = 0; i < list.size(); i++) {
        if ((list[i]->tIndex != uignore1) && (list[i]->tIndex != uignore2)) {
            triangleTests++;
            if (list[i]->isIntersectedByRay())
                return true;
        }
    }

    /// subnodes test
    for (int j = 0; j < 8; j++) {
        if (hasNext[j]) {
            AABBTests++;
            if (TestSegmentAABB(next[j]->reg)) {
                if (next[j]->isIntersected())
                    return true;
            }
        }
    }

    /// no intersection found
    return false;
}
