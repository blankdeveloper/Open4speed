#include <glm/gtx/intersect.hpp>
#include "raycaster/triangle.h"
#include "raycaster/utils.h"
#include "common.h"

triangle* lastIntersectedTriangle = 0;

triangle::triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec2 aID, glm::vec2 bID, glm::vec2 cID, int lmIndex, int tIndex) {
    this->a = glm::vec3(a);
    this->b = glm::vec3(b);
    this->c = glm::vec3(c);
    this->aID = glm::vec2(aID.x * rttsize, aID.y * rttsize);
    this->bID = glm::vec2(bID.x * rttsize, bID.y * rttsize);
    this->cID = glm::vec2(cID.x * rttsize, cID.y * rttsize);
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
    glm::vec3 A = glm::vec3(0, 0, 1);
    glm::vec3 B = glm::vec3(1, 0, 1);
    glm::vec3 C = glm::vec3(0, 1, 1);
    glm::vec3 P = glm::vec3(u / 255.0f, v / 255.0f, 1);
    float area = triangleArea(A, B, C);
    float lena = triangleArea(P, B, C) / area;
    float lenb = triangleArea(P, A, C) / area;
    float lenc = triangleArea(P, A, B) / area;

    glm::vec3 vert = a * lena + b * lenb + c * lenc;
    glm::vec2 t = aID * lena + bID * lenb + cID * lenc;
    points.push_back({vert, (glm::ivec2)t});
}

glm::vec3 p = glm::vec3(0,0,0);

bool triangle::isIntersectedByRay() {
    bool intersected = glm::intersectLineTriangle(ubegin, uraydir, a, b, c, p);
    if (intersected)
        lastIntersectedTriangle = this;
    return intersected;
}
