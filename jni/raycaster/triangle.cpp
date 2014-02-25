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
    reg.min.x = 9999999;
    reg.min.y = 9999999;
    reg.min.z = 9999999;
    reg.max.x = -9999999;
    reg.max.y = -9999999;
    reg.max.z = -9999999;
    addPointToAABB(a);
    addPointToAABB(b);
    addPointToAABB(c);
    reg.size = fabsf(reg.max.x - reg.min.x);
    if (reg.size < fabsf(reg.max.y - reg.min.y))
        reg.size = fabsf(reg.max.y - reg.min.y);
    if (reg.size < fabsf(reg.max.z - reg.min.z))
        reg.size = fabsf(reg.max.z - reg.min.z);
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

glm::vec3 triangle::getPoint(int u, int v) {
    float x = u / 255.0f;
    float y = v / 255.0f;
    return a + x * (b - a) + y * (c - a);
}

bool triangle::isIntersectedByRay(glm::vec3 point, glm::vec3 direction) {
    glm::vec3 e1 = b - a;
    glm::vec3 e2 = c - a;
    glm::vec3 s1 = glm::cross(direction, e2);

    float divisor = glm::dot(s1, e1);

    if (divisor < 0.01f)
        return false;

    /// Compute first barycentric coordinate
    glm::vec3 d = point - a;
    float b1 = glm::dot(d, s1);
    if (b1 < 0. || b1 > divisor)
        return false;
    /// Compute second barycentric coordinate
    glm::vec3 s2 = glm::cross(d, e1);
    float b2 = glm::dot(direction, s2);
    if (b2 < 0. || b1 + b2 > divisor)
        return false;

    /// Compute _t_ to intersection point
    if (glm::dot(e2, s2) / divisor > 0) {
        return true;
    }
    return false;
}

bool triangle::isIntersectedBySegment(glm::vec3 begin, glm::vec3 end) {
    return isIntersectedByRay(begin, end - begin) && isIntersectedByRay(end, begin - end);
}
