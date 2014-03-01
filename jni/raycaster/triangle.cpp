#include "raycaster/triangle.h"
#include "utils/math.h"
#include "common.h"

triangle::triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec2 aID, glm::vec2 bID, glm::vec2 cID, int lmIndex) {
    this->a = glm::vec3(a);
    this->b = glm::vec3(b);
    this->c = glm::vec3(c);
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
    double x = u / 255.0f;
    double y = v / 255.0f;
    if (x > y) {
        double b1 = -(x - 1.0);
        double b2 = (x - 1.0) - (y - 1.0);
        double b3 = 1.0 - b1 - b2;
        return (glm::vec3)(glm::dvec3(a) * b1 + glm::dvec3(b) * b2 + glm::dvec3(c) * b3);
    }
    else {
        double b1 = -(y - 1.0);
        double b2 = -((x - 1.0) - (y - 1.0));
        double b3 = 1.0 - b1 - b2;
        return (glm::vec3)(glm::dvec3(a) * b1 + glm::dvec3(b) * b2 + glm::dvec3(c) * b3);
    }
}

bool triangle::isIntersectedByRay(glm::vec3 rayOrig, glm::vec3 rayDir) {
    // compute plane's normal
    glm::vec3 A = b - a;
    glm::vec3 B = c - a;
    // no need to normalize
    glm::vec3 N = glm::cross(A, B); // N

    // check if ray and plane are parallel ?
    float NdotRayDirection = glm::dot(N, rayDir);
    if (NdotRayDirection == 0)
        return false; // they are parallel so they don't intersect !
    // compute t (equation 3)
    float t = -(glm::dot(N, rayOrig) + glm::dot(N, a)) / NdotRayDirection;
    // check if the triangle is in behind the ray
    if ((t < 0) || (t > 1))
        return false; // the triangle is behind
    // compute the intersection point using equation 1
    glm::vec3 P = rayOrig + t * rayDir;

    // edge 0
    if (glm::dot(N, glm::cross(b - a, P - a)) < 0)
        return false; // P is on the right side

    // edge 1
    if (glm::dot(N, glm::cross(c - b, P - b)) < 0)
        return false; // P is on the right side

    // edge 2
    if (glm::dot(N, glm::cross(a - c, P - c)) < 0)
        return false; // P is on the right side;

    return true; // this ray hits the triangle
}
