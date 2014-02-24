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

/**
 * @brief octreeNode is a constructor of root node
 * @param size is size of a world
 * @param m is model to add
 */
octreeNode::octreeNode(AABB *r) {
    reg = r;
    for (int i = 0; i < 8; i++) {
        hasNext[i] = false;
    }
}

/**
 * @brief octreeNode is constructor for subnodes
 * @param r is subnode region
 */
octreeNode::octreeNode(float size, model *m) {
    /// Set limits of tree
    reg = new AABB();
    reg->minX = -size / 2;
    reg->minY = -size / 2;
    reg->minZ = -size / 2;
    reg->maxX = size / 2;
    reg->maxY = size / 2;
    reg->maxZ = size / 2;
    reg->size = size;

    for (int i = 0; i < 8; i++) {
        hasNext[i] = false;
    }

    /// Add top models
    for (unsigned int i = 0; i < m->models.size(); i++) {
        m->models[i].fixed = false;
        data d = {&m->models[i], 0};
        list.push_back(d);
    }

    createSubNodes();
}

/**
 * @brief createSubNodes creates subnodes of current node
 */
void octreeNode::createSubNodes() {
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
        if (!list[i].model->fixed) {
            bool moved = false;
            for (int j = 0; j < 8; j++) {
                bool ok = true;
                if (r[j]->size < list[i].model->reg->size / 2.0f) {
                    ok = false;
                }
                if (r[j]->minX > list[i].model->reg->maxX + list[i].model->x - list[i].model->reg->minX) {
                    ok = false;
                }
                if (r[j]->maxX < list[i].model->reg->minX + list[i].model->x - list[i].model->reg->minX) {
                    ok = false;
                }
                if (r[j]->minY > list[i].model->reg->maxY + list[i].model->y - list[i].model->reg->minY) {
                    ok = false;
                }
                if (r[j]->maxY < list[i].model->reg->minY + list[i].model->y - list[i].model->reg->minY) {
                    ok = false;
                }
                if (r[j]->minZ > list[i].model->reg->maxZ + list[i].model->z - list[i].model->reg->minZ) {
                    ok = false;
                }
                if (r[j]->maxZ < list[i].model->reg->minZ + list[i].model->z - list[i].model->reg->minZ) {
                    ok = false;
                }
                if (ok) {
                    if (!hasNext[j]) {
                        next[j] = new octreeNode(r[j]);
                        hasNext[j] = true;
                    }
                    next[j]->list.push_back(list[i]);
                    moved = true;
                }

            }
            if (!moved) {
                list[i].model->fixed = true;
            } else {
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

/**
 * @brief getSubregion gets subregion
 * @param x is position coordinate
 * @param y is position coordinate
 * @param z is position coordinate
 * @return instance of region structure
 */
AABB* octreeNode::getSubregion(bool x, bool y, bool z) {
    AABB *r = new AABB;
    if (x) {
        r->minX = reg->minX + reg->size / 2;
        r->maxX = reg->maxX;
    } else {
        r->minX = reg->minX;
        r->maxX = reg->maxX - reg->size / 2;
    }
    if (y) {
        r->minY = reg->minY + reg->size / 2;
        r->maxY = reg->maxY;
    } else {
        r->minY = reg->minY;
        r->maxY = reg->maxY - reg->size / 2;
    }
    if (z) {
        r->minZ = reg->minZ + reg->size / 2;
        r->maxZ = reg->maxZ;
    } else {
        r->minZ = reg->minZ;
        r->maxZ = reg->maxZ - reg->size / 2;
    }
    r->size = reg->size / 2;
    return r;
}
