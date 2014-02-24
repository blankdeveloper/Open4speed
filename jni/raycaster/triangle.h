#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <glm/glm.hpp>

class triangle
{
public:
    glm::vec3 a;
    glm::vec3 b;
    glm::vec3 c;
    glm::ivec2 aID;
    glm::ivec2 bID;
    glm::ivec2 cID;
    int lmIndex;

    triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec2 aID, glm::vec2 bID, glm::vec2 cID, int lmIndex);

    glm::vec3 getPoint(int u, int v);
};

#endif // TRIANGLE_H
