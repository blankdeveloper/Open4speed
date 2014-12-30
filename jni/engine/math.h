///----------------------------------------------------------------------------------------
/**
 * \file       math.h
 * \author     Vonasek Lubos
 * \date       2014/12/30
 * \brief      Common math utils used in program.
**/
///----------------------------------------------------------------------------------------

#ifndef MATH_H
#define MATH_H

#include <glm/glm.hpp>
#include <vector>

/**
 * @brief The edge struct
 */
struct edge {
    glm::vec3 a; ///< Point coordinates
    glm::vec3 b; ///< Point coordinates
};

/**
 * @brief AABB struct
 */
struct AABB {
    glm::vec3 min; ///< Region extreme
    glm::vec3 max; ///< Region extreme
};

/**
 * @brief angle counts angle of line between two points given by coordinates
 * @param fromX is first point coordinate x
 * @param fromY is first point coordinate y
 * @param toX is second point coordinate x
 * @param toY is second point coordinate y
 * @return angle in radians
 */
float angle(float fromX, float fromY, float toX, float toY);

/**
 * @brief angle counts angle between two cars
 * @param a is first position
 * @param b is second position
 * @return angle in radians
 */
float angle(glm::vec3 a, glm::vec3 b);

/**
 * @brief distance counts distance between car and of the end edge
 * @param a is first position
 * @param b is second position
 * @return distance in float
 */
float distance(glm::vec3 a, glm::vec3 b);

/**
 * @brief distance count distance between two points given by coordinates
 * @param fromX is first point coordinate x
 * @param fromY is first point coordinate y
 * @param toX is second point coordinate x
 * @param toY is second point coordinate y
 * @return distance in float
 */
float distance(float fromX, float fromY, float toX, float toY);

/**
 * @brief gap counts gap between car and of the end edge
 * @param e is instance of edge
 * @param c is instance of car
 * @return gap in radians
 */
float gap(glm::vec3 e, glm::vec3 c, float crot);

/**
 * @brief getRotation gets rotation of Y axis from transformation matrix
 * @param x is position coordinate
 * @param y is position coordinate
 * @param z is position coordinate
 * @param w is weight coordinate
 * @return value in degress
 */
float getRotation(float x, float y, float z, float w);

/**
 * @brief isSame check if two edges are the same
 * @param a is the first edge
 * @param b is the second edge
 * @return true if edges are same
 */
bool isSame(edge a, edge b);

/**
 * @brief nextEdge gets all possible indexes of next edges
 * @param edges is track curve instance
 * @param e is current edge
 * @return indicies as vector of int
 */
std::vector<int> nextEdge(std::vector<edge> edges, edge e);

#endif // MATH_H
