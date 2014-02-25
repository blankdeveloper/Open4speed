#include "raycaster/triangle.h"
#include "utils/math.h"
#include "common.h"

triangle::triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec2 aID, glm::vec2 bID, glm::vec2 cID, int lmIndex) {
    this->a = a;
    this->b = b;
    this->c = c;
    this->aID = glm::ivec2(aID.x * rttsize, aID.y * rttsize);
    this->bID = glm::ivec2(bID.x * rttsize, bID.y * rttsize);
    this->cID = glm::ivec2(cID.x * rttsize, cID.y * rttsize);
    this->lmIndex = lmIndex;
    reg.minX = 9999999;
    reg.minY = 9999999;
    reg.minZ = 9999999;
    reg.maxX = -9999999;
    reg.maxY = -9999999;
    reg.maxZ = -9999999;
    addPointToAABB(a);
    addPointToAABB(b);
    addPointToAABB(c);
    reg.size = absf(reg.maxX - reg.minX);
    if (reg.size < absf(reg.maxY - reg.minY))
        reg.size = absf(reg.maxY - reg.minY);
    if (reg.size < absf(reg.maxZ - reg.minZ))
        reg.size = absf(reg.maxZ - reg.minZ);
}


void triangle::addPointToAABB(glm::vec3 p) {
    if (reg.minX > p.x)
        reg.minX = p.x;
    if (reg.minY > p.y)
        reg.minY = p.y;
    if (reg.minZ > p.z)
        reg.minZ = p.z;

    if (reg.maxX < p.x)
        reg.maxX = p.x;
    if (reg.maxY < p.y)
        reg.maxY = p.y;
    if (reg.maxZ < p.z)
        reg.maxZ = p.z;
}

glm::vec3 triangle::getPoint(int u, int v) {
    float x = u / 255.0f;
    float y = v / 255.0f;
    return a + x * (b - a) + y * (c - a);
}
