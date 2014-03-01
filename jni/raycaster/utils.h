#ifndef UTILS_H
#define UTILS_H

#include <glm/glm.hpp>
#include "interfaces/model.h"

extern glm::vec3 ubegin;
extern glm::vec3 uend;
extern glm::vec3 uraydir;
extern long uignore1;
extern long uignore2;
extern long utestID;

void setUniforms(glm::vec3 begin, glm::vec3 end, long ignore1, long ignore2, long testID);

void startTimer();

void stopTimer();

float triangleArea(glm::vec3 a, glm::vec3 b, glm::vec3 c);

bool TestAABBAABB(AABB* a, AABB* b);

// Test if segment intersects AABB b
int TestSegmentAABB(AABB* b);

#endif // UTILS_H
