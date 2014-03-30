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

struct Uniform {
    glm::vec3 begin;
    glm::vec3 end;
    glm::vec3 raydir;
    long ignore1;
    long ignore2;
    long testID;
    glm::vec3 L;
};

extern Uniform *uniform;

float countLightMaxDistance(float att);

glm::vec4 getColor(Point p);

void setUniforms(glm::vec3 begin, glm::vec3 end, long ignore1, long ignore2, long testID);

void startTimer();

void stopTimer();

glm::vec3 swizle(glm::vec4 v);

float triangleArea(glm::vec3 a, glm::vec3 b, glm::vec3 c);

bool TestAABBAABB(AABB* a, AABB* b);

// Test if segment intersects AABB b
int TestSegmentAABB(AABB* b);

#endif // UTILS_H
