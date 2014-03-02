#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <glm/glm.hpp>
#include "interfaces/model.h"

struct Point {
    glm::vec3 v;
    glm::ivec2 t;
    glm::vec3 n;
    glm::vec2 c;
};

class triangle
{
public:
    AABB reg;           ///< AABB of the object
    glm::vec3 a;
    glm::vec3 b;
    glm::vec3 c;
    glm::ivec2 aID;
    glm::ivec2 bID;
    glm::ivec2 cID;
    glm::vec3 na;
    glm::vec3 nb;
    glm::vec3 nc;
    glm::vec2 ta;
    glm::vec2 tb;
    glm::vec2 tc;
    int lmIndex;
    long tIndex;
    long lastTestID;
    std::vector<Point> points;

    triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c,
             glm::vec2 aID, glm::vec2 bID, glm::vec2 cID,
             glm::vec3 na, glm::vec3 nb, glm::vec3 nc,
             glm::vec2 ta, glm::vec2 tb, glm::vec2 tc,
             int lmIndex, long tIndex);

    void addPointToAABB(glm::vec3 p);

    void addLMPoint(int u, int v, glm::ivec2 t);

    bool isIntersectedByRay();
};

extern triangle* lastIntersectedTriangle;
extern triangle* lastIntersectedTriangle2;

#endif // TRIANGLE_H
