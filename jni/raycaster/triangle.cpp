#include "raycaster/triangle.h"
#include "common.h"

triangle::triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec2 aID, glm::vec2 bID, glm::vec2 cID, int lmIndex) {
    this->a = a;
    this->b = b;
    this->c = c;
    this->aID = glm::ivec2(aID.x * rttsize, aID.y * rttsize);
    this->bID = glm::ivec2(bID.x * rttsize, bID.y * rttsize);
    this->cID = glm::ivec2(cID.x * rttsize, cID.y * rttsize);
    this->lmIndex = lmIndex;
}

glm::vec3 triangle::getPoint(int u, int v) {
    float x = u / 255.0f;
    float y = v / 255.0f;
    return a + x * (b - a) + y * (c - a);
}
