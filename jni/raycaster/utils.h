#ifndef UTILS_H
#define UTILS_H

#include <glm/glm.hpp>
#include "interfaces/model.h"

/// point info
struct Point {
    glm::vec3 v;
    glm::ivec2 t;
    glm::vec3 n;
    glm::vec2 c;
    glm::vec3 bary;
    glm::vec2 uv;
};

/// point light structure
struct PLP {
    glm::vec4 pos;
    glm::vec4 dir;
    glm::vec4 color;
    bool useable;
};

/**
 * countLightMaxDistance counts maximal distance of light
 * @param att is quadratic attenuation
 * @return distance
 */
float countLightMaxDistance(float att);

/**
 * getColor gets color of 3D point for current point ray
 * @param p is 3D point
 * @param raybegin is ray begin
 * @param rayend is ray end
 * @return RGB color + intensity in A channel
 */
glm::vec4 getColor(Point *p, glm::vec3 raybegin, glm::vec3 rayend);

/**
 * PointInTriangle check if 2D point is in triangle
 * @param pt is point to check
 * @param v1 is first vertex of triangle
 * @param v2 is second vertex of triangle
 * @param v3 is triangle vertex of triangle
 * @return true if it is inside
 */
bool PointInTriangle(glm::ivec3 pt, glm::ivec3 v1, glm::ivec3 v2, glm::ivec3 v3);

/**
 * startTimer starts timer
 */
void startTimer();

/**
 * stopTimer stops timer
 */
void stopTimer();

/**
 * swizle swizles vec4 to vec3
 * @param v is vec4
 * @return vec3
 */
glm::vec3 swizle(glm::vec4 v);

/**
 * TestAABBAABB test if two AABBs intersecting
 * @param a is first AABB
 * @param b is second AABB
 * @return true if it is intersecting
 */
bool TestAABBAABB(AABB* a, AABB* b);

/**
 * TestSegmentAABB tests if segment intersects AABB b
 * @param b is AABB to test
 * @param raybegin is ray begin
 * @param rayend is ray end
 * @return 1 if segment intersects AABB
 */
int TestSegmentAABB(AABB* b, glm::vec3 raybegin, glm::vec3 rayend);

/**
 * triangleArea counts area of triangle
 * @param a is first point of triangle
 * @param b is second point of triangle
 * @param c is third point of triangle
 * @return area of triangle
 */
float triangleArea(glm::vec3 a, glm::vec3 b, glm::vec3 c);

#endif // UTILS_H
