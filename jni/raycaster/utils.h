#ifndef UTILS_H
#define UTILS_H

#include <glm/glm.hpp>
#include "interfaces/model.h"

struct Point {
    glm::vec3 v;
    glm::ivec2 t;
    glm::vec3 n;
    glm::vec2 c;
};

struct PLP {
    glm::vec4 pos;
    glm::vec4 dir;
    glm::vec4 color;
    bool useable;
};

float countLightMaxDistance(float att);

glm::vec4 getColor(Point *p, glm::vec3 raybegin, glm::vec3 rayend);

void startTimer();

void stopTimer();

glm::vec3 swizle(glm::vec4 v);

float triangleArea(glm::vec3 a, glm::vec3 b, glm::vec3 c);

bool TestAABBAABB(AABB* a, AABB* b);

// Test if segment intersects AABB b
int TestSegmentAABB(AABB* b, glm::vec3 raybegin, glm::vec3 rayend);

#endif // UTILS_H
