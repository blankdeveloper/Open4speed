#ifndef UTILS_H
#define UTILS_H

#include <glm/glm.hpp>

extern glm::vec3 ubegin;
extern glm::vec3 uend;
extern glm::vec3 uraydir;
extern int uignore1;
extern int uignore2;
extern long utestID;

/**
 * @brief aabbSegmentIntersection check if AABB and line segment intersects
 * @param p1 is begin of line segment
 * @param p2 is end of line segment
 * @param min is AABB minimum
 * @param max is AABB maximum
 * @return true if it is intersecting
 */
bool aabbSegmentIntersection(glm::vec3 min, glm::vec3 max);

void setUniforms(glm::vec3 begin, glm::vec3 end, int ignore1, int ignore2, long testID);

float triangleArea(glm::vec3 a, glm::vec3 b, glm::vec3 c);

#endif // UTILS_H
