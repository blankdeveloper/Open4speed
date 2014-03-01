#include <glm/gtx/intersect.hpp>
#include "raycaster/triangle.h"
#include "raycaster/utils.h"
#include "utils/math.h"
#include "common.h"

triangle* lastIntersectedTriangle = 0;

triangle::triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec2 aID, glm::vec2 bID, glm::vec2 cID, int lmIndex, int tIndex) {
    this->a = glm::vec3(a);
    this->b = glm::vec3(b);
    this->c = glm::vec3(c);
    this->aID = glm::ivec2(aID.x * rttsize, aID.y * rttsize);
    this->bID = glm::ivec2(bID.x * rttsize, bID.y * rttsize);
    this->cID = glm::ivec2(cID.x * rttsize, cID.y * rttsize);
    this->lmIndex = lmIndex;
    this->tIndex = tIndex;
    lastTestID = -1;
    reg.min.x = 9999999;
    reg.min.y = 9999999;
    reg.min.z = 9999999;
    reg.max.x = -9999999;
    reg.max.y = -9999999;
    reg.max.z = -9999999;
    addPointToAABB(a);
    addPointToAABB(b);
    addPointToAABB(c);
}


void triangle::addPointToAABB(glm::vec3 p) {
    if (reg.min.x > p.x)
        reg.min.x = p.x;
    if (reg.min.y > p.y)
        reg.min.y = p.y;
    if (reg.min.z > p.z)
        reg.min.z = p.z;

    if (reg.max.x < p.x)
        reg.max.x = p.x;
    if (reg.max.y < p.y)
        reg.max.y = p.y;
    if (reg.max.z < p.z)
        reg.max.z = p.z;
}

void triangle::addLMPoint(int u, int v) {
    double x = u / 255.0f;
    double y = v / 255.0f;
    double b1 = -(x - 1.0);
    double b2;
    double b3 = 1.0 - b1 - b2;
    if (x > y)
        b2 = (x - 1.0) - (y - 1.0);
    else
        b2 = -((x - 1.0) - (y - 1.0));
    glm::vec3 vert = (glm::vec3)(glm::dvec3(a) * b1 + glm::dvec3(b) * b2 + glm::dvec3(c) * b3);
    glm::ivec2 t = (glm::ivec2)(glm::dvec2(aID) * b1 + glm::dvec2(bID) * b2 + glm::dvec2(cID) * b3);
    points.push_back({vert, t});
}

glm::vec3 p = glm::vec3(0,0,0);

bool triangle::isIntersectedByRay() {
    bool intersected = glm::intersectLineTriangle(ubegin, uraydir, a, b, c, p);
    if (intersected)
        lastIntersectedTriangle = this;
    return intersected;
}
