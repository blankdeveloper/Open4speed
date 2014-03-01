#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <glm/glm.hpp>
#include "interfaces/model.h"

struct Point {
    glm::vec3 v;
    glm::ivec2 t;
};

class triangle
{
public:
    AABB reg;           ///< AABB of the object
    glm::vec3 a;
    glm::vec3 b;
    glm::vec3 c;
    glm::vec2 aID;
    glm::vec2 bID;
    glm::vec2 cID;
    int lmIndex;
    int tIndex;
    long lastTestID;
    std::vector<Point> points;

    triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec2 aID, glm::vec2 bID, glm::vec2 cID, int lmIndex, int tIndex);

    void addPointToAABB(glm::vec3 p);

    void addLMPoint(int u, int v, glm::ivec2 t);

    bool isIntersectedByRay();
};

extern triangle* lastIntersectedTriangle;

#endif // TRIANGLE_H
