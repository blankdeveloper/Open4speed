#include "raycaster/utils.h"


const float EPSILON = 0.00001f;

glm::vec3 ubegin;
glm::vec3 uend;
glm::vec3 uraydir;
int uignore1;
int uignore2;
long utestID;

/**
 * @brief aabbSegmentIntersection check if AABB and line segment intersects
 * @param min is AABB minimum
 * @param max is AABB maximum
 * @return true if it is intersecting
 */
bool aabbSegmentIntersection(glm::vec3 min, glm::vec3 max) {
    glm::vec3 d = (uend - ubegin) * 0.5f;
    glm::vec3 e = (max - min) * 0.5f;
    glm::vec3 c = ubegin + d - (min + max) * 0.5f;
    glm::vec3 ad = glm::vec3(fabs(d.x), fabs(d.y), fabs(d.z));

    if (fabsf(c[0]) > e[0] + ad[0])
        return false;
    if (fabsf(c[1]) > e[1] + ad[1])
        return false;
    if (fabsf(c[2]) > e[2] + ad[2])
        return false;

    if (fabsf(d[1] * c[2] - d[2] * c[1]) > e[1] * ad[2] + e[2] * ad[1] + EPSILON)
        return false;
    if (fabsf(d[2] * c[0] - d[0] * c[2]) > e[2] * ad[0] + e[0] * ad[2] + EPSILON)
        return false;
    if (fabsf(d[0] * c[1] - d[1] * c[0]) > e[0] * ad[1] + e[1] * ad[0] + EPSILON)
        return false;

    return true;
}

void setUniforms(glm::vec3 begin, glm::vec3 end, int ignore1, int ignore2, long testID) {
    ubegin = begin;
    uend = end;
    uignore1 = ignore1;
    uignore2 = ignore2;
    uraydir = end - begin;
    utestID = testID;
}
