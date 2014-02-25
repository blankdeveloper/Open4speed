#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <glm/glm.hpp>
#include "interfaces/model.h"

class triangle
{
public:
    AABB reg;           ///< AABB of the object
    bool fixed;         ///< Raycasting flag
    glm::vec3 a;
    glm::vec3 b;
    glm::vec3 c;
    glm::ivec2 aID;
    glm::ivec2 bID;
    glm::ivec2 cID;
    int lmIndex;

    triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec2 aID, glm::vec2 bID, glm::vec2 cID, int lmIndex);

    void addPointToAABB(glm::vec3 p);

    glm::vec3 getPoint(int u, int v);

    bool isIntersectedByRay(glm::vec3 point, glm::vec3 direction);

    bool isIntersectedBySegment(glm::vec3 begin, glm::vec3 end);
};

#endif // TRIANGLE_H
