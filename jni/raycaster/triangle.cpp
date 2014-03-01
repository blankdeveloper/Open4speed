#include "raycaster/triangle.h"
#include "raycaster/utils.h"
#include "common.h"

triangle* lastIntersectedTriangle = 0;

triangle::triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec2 aID, glm::vec2 bID, glm::vec2 cID, int lmIndex, int tIndex) {
    this->a = glm::vec3(a);
    this->b = glm::vec3(b);
    this->c = glm::vec3(c);
    this->aID = glm::vec2(aID.x * rttsize, aID.y * rttsize) + 0.5f;
    this->bID = glm::vec2(bID.x * rttsize, bID.y * rttsize) + 0.5f;
    this->cID = glm::vec2(cID.x * rttsize, cID.y * rttsize) + 0.5f;
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

void triangle::addLMPoint(int u, int v, glm::ivec2 t) {
    glm::vec3 A = glm::vec3(0, 0, 1);
    glm::vec3 B = glm::vec3(1, 0, 1);
    glm::vec3 C = glm::vec3(0, 1, 1);
    glm::vec3 P = glm::vec3(u / 255.0f, v / 255.0f, 1);
    float area = triangleArea(A, B, C);
    float lena = triangleArea(P, B, C) / area;
    float lenb = triangleArea(P, A, C) / area;
    float lenc = triangleArea(P, A, B) / area;

    glm::vec3 vert = a * lena + b * lenb + c * lenc;
    points.push_back({vert, t});
}

float ScalarTriple(glm::vec3 pq, glm::vec3 pc, glm::vec3 pb) {
    return glm::dot(pb, glm::cross(pq, pc));
}

bool triangle::isIntersectedByRay() {
    glm::vec3 pq = uend - ubegin;
    glm::vec3 pa = a - ubegin;
    glm::vec3 pb = b - ubegin;
    glm::vec3 pc = c - ubegin;
    // Test if pq is inside the edges bc, ca and ab. Done by testing
    // that the signed tetrahedral volumes, computed using scalar triple
    // products, are all positive
    if (ScalarTriple(pq, pc, pb) < 0.0f) return 0;
    if (ScalarTriple(pq, pa, pc) < 0.0f) return 0;
    if (ScalarTriple(pq, pb, pa) < 0.0f) return 0;

    lastIntersectedTriangle = this;
    return true;
}
