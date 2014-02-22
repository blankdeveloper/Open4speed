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

int currentCycle = 0;               ///< Current cycle
bool OPT_MAILBOXING = true;         ///< Mailboxing switch

std::vector<model3d*> toRender;     ///< Nodes to render

/**
 * @brief octreeNode is a constructor of root node
 * @param size is size of a world
 * @param m is model to add
 */
octreeNode::octreeNode(region *r) {
    reg = r;
    distance = 0;
    lastUpdate = 0;
    previous_matrix = xrenderer->proj_matrix * xrenderer->view_matrix;

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
    reg = new region();
    reg->minX = -size / 2;
    reg->minY = -size / 2;
    reg->minZ = -size / 2;
    reg->maxX = size / 2;
    reg->maxY = size / 2;
    reg->maxZ = size / 2;
    reg->size = size;

    distance = 0;
    lastUpdate = 0;
    previous_matrix = xrenderer->proj_matrix * xrenderer->view_matrix;

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
 * @brief basicTest tests AABB with frustm planes
 * @return 0 if it is outside, 1 if it is partitionaly inside, 2 if it is inside
 */
int octreeNode::basicTest() {

    /// Basic Intersection Test
    int x1 = 0; int x2 = 0; int y1 = 0; int y2 = 0; int z1 = 0; int z2 = 0;
    for (int i = 0; i < 8; i++) {
        /// check if it is inside
        glm::vec4 vec = getTransform(i);
        if (vec.x <= 1) { x1++; }
        if (vec.x >= -1) { x2++; }
        if (vec.y <= 1) { y1++; }
        if (vec.y >= -1) { y2++; }
        if (vec.z <= 1) { z1++; }
        if (vec.z >= -1) { z2++; }
    }

    /// return value
    if (x1 + y1 + x2 + y2 + z1 + z2 == 48) {
        return 2;
    } else if ((x1 > 0) & (x2 > 0) & (y1 > 0) & (y2 > 0) & (z1 > 0) & (z2 > 0)) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * @brief createSubNodes creates subnodes of current node
 */
void octreeNode::createSubNodes() {
    region* r[8];
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
region* octreeNode::getSubregion(bool x, bool y, bool z) {
    region *r = new region;
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

/**
 * @brief getTransform gets position of AABB vertex in frustum space
 * @param i is index of vertex
 * @return transformed vertex in frustum space
 */
glm::vec4 octreeNode::getTransform(int i) {
    if (lastUpdate != currentCycle) {
        glm::mat4x4 view_projection_matrix = xrenderer->proj_matrix * xrenderer->view_matrix;
        position[0] = view_projection_matrix * glm::vec4(reg->minX, reg->minY, reg->minZ, 1);
        position[1] = view_projection_matrix * glm::vec4(reg->minX, reg->minY, reg->maxZ, 1);
        position[2] = view_projection_matrix * glm::vec4(reg->minX, reg->maxY, reg->minZ, 1);
        position[3] = view_projection_matrix * glm::vec4(reg->minX, reg->maxY, reg->maxZ, 1);
        position[4] = view_projection_matrix * glm::vec4(reg->maxX, reg->minY, reg->minZ, 1);
        position[5] = view_projection_matrix * glm::vec4(reg->maxX, reg->minY, reg->maxZ, 1);
        position[6] = view_projection_matrix * glm::vec4(reg->maxX, reg->maxY, reg->minZ, 1);
        position[7] = view_projection_matrix * glm::vec4(reg->maxX, reg->maxY, reg->maxZ, 1);
        for (int j = 0; j < 8; j++) {
            position[j].x = position[j].x / position[j].w;
            position[j].y = position[j].y / position[j].w;
            position[j].z = position[j].z / viewDistance * 2 - 1;
        }
        lastUpdate = currentCycle;
    }
    return position[i];
}

/**
 * @brief process processes node and subnodes
 */
void octreeNode::process() {

    /// update dynamic objects
    if (update) {
        distance = 0;
        update = false;
        createSubNodes();
    }

    /// add models of current node into render list
    for (unsigned int i = 0; i < list.size(); i++) {
        toRender.push_back(list[i].model);
    }

    // Basic Intersection Test
    int bTest = basicTest();

    ///  Mailboxing
    if (OPT_MAILBOXING & (bTest == 2)) {
        processAll();
    }

    ///  The Octant Test
    else if (bTest == 1) {
        for (int i = 0; i < 8; i++) {
            if (hasNext[i]) {
                next[i]->process();
            }
        }
    }
}

/**
 * @brief processAll adds all into to render list
 */
void octreeNode::processAll() {
    for (unsigned int i = 0; i < list.size(); i++) {
        toRender.push_back(list[i].model);
    }
    for (int i = 0; i < 8; i++) {
        if (hasNext[i]) {
            next[i]->processAll();
        }
    }
}

/**
 * @brief render renders current node
 * @param renderer is instance of renderer
 * @param gamma is requested amount of gamma
 */
void octreeNode::render(renderer* renderer, float gamma) {
    currentCycle++;
    toRender.clear();
    process();
    for (unsigned int i = 0; i < toRender.size(); i++) {
        renderer->renderSubModel(trackdata, toRender[i]);
    }
}
