//----------------------------------------------------------------------------------------
/**
 * \file       math.cpp
 * \author     Vonasek Lubos
 * \date       2014/11/01
 * \brief      Common math utils used in program.
*/
//----------------------------------------------------------------------------------------

#include "utils/math.h"

/**
 * @brief angle counts angle of line between two points given by coordinates
 * @param fromX is first point coordinate x
 * @param fromY is first point coordinate y
 * @param toX is second point coordinate x
 * @param toY is second point coordinate y
 * @return angle in radians
 */
float angle(float fromX, float fromY, float toX, float toY) {
    /// solve special cases
    if (fromY == toY) {
        if (toX > fromX) {
            return 1.57 + 3.14;
        } else {
            return 1.57;
        }
    }

    /// solve standart cases
    float angle = atan((fromX - toX) / (fromY - toY));
    if (fromY - toY < 0)
        angle -= 3.14;
    return angle;
}

/**
 * @brief angle counts angle between two cars
 * @param a is first position
 * @param b is second position
 * @return angle in radians
 */
float angle(glm::vec3 a, glm::vec3 b) {
    return angle(a.x, a.z, b.x, b.z);
}

/**
 * @brief distance counts distance between car and of the end edge
 * @param a is first position
 * @param b is second position
 * @return distance in float
 */
float distance(glm::vec3 a, glm::vec3 b) {
    return distance(a.x, a.z, b.x, b.z);
}

/**
 * @brief distance count distance between two points given by coordinates
 * @param fromX is first point coordinate x
 * @param fromY is first point coordinate y
 * @param toX is second point coordinate x
 * @param toY is second point coordinate y
 * @return distance in float
 */
float distance(float fromX, float fromY, float toX, float toY) {
    return sqrt((sqr(fromX - toX) + sqr(fromY - toY)));
}

/**
 * @brief gap counts gap between car and of the end edge
 * @param e is instance of edge
 * @param c is instance of car
 * @return gap in radians
 */
float gap(glm::vec3 e, glm::vec3 c, float crot) {
    float direction = angle(c, e) * 180 / 3.14;
    float gap = crot - direction - 180;
    while(true) {
        if (gap < -180)
            gap += 360;
        else if (gap > 180)
            gap -= 360;
        else
            break;
        }
    return gap;
}

/**
 * @brief getRotation gets rotation of Y axis from transformation matrix
 * @param x is position coordinate
 * @param y is position coordinate
 * @param z is position coordinate
 * @param w is weight coordinate
 * @return value in degress
 */
float getRotation(float x, float y, float z, float w) {
    float rot = asin(-2.0 * (x*z - y*w)) * 180 / 3.14;
    if (cos(atan2((float)(2.0 * (y*z + x*w)),(-sqr(x) - sqr(y) + sqr(z) + sqr(w)))) < 0) {
        rot = 180 - rot;
    }
    return rot;
}

/**
 * @brief isSame check if two edges are the same
 * @param a is the first edge
 * @param b is the second edge
 * @return true if edges are same
 */
bool isSame(edge a, edge b) {
    if ((a.a.x == b.b.x) & (a.a.z == b.b.z))
        if ((b.a.x == a.b.x) & (b.a.z == a.b.z))
            return true;
    if ((a.a.x == b.a.x) & (a.a.z == b.a.z))
        if ((b.b.x == a.b.x) & (b.b.z == a.b.z))
            return true;
    return false;
}

/**
 * @brief max get max value of input
 * @param a is first value
 * @param b is second value
 * @return bigger value
 */
int max(int a, int b) {
    if (a > b)
        return a;
    else
        return b;
}

/**
 * @brief min get min value of input
 * @param a is first value
 * @param b is second value
 * @return smaller value
 */
int min(int a, int b) {
    if (a > b)
        return b;
    else
        return a;
}

/**
 * @brief nextEdge gets all possible indexes of next edges
 * @param edges is track curve instance
 * @param e is current edge
 * @return indicies as vector of int
 */
std::vector<int> nextEdge(std::vector<edge> edges, edge e) {
    std::vector<int> output;

    /// skip logical edges
    /*for (unsigned int i = 0; i < edges.size(); i++) {
        if ((edges[i].a.x == e.b.x) && (edges[i].a.z == e.b.z)) {
            if (e.b.y > edges[i].a.y) {
                e.b.y = edges[i].a.y;
            }
        }
    }*/

    /// add possible edges
    for (unsigned int i = 0; i < edges.size(); i++) {
        if ((edges[i].a.x == e.b.x) & (edges[i].a.y == e.b.y) & (edges[i].a.z == e.b.z)) {
            if (!isSame(edges[i],e))
                if ((edges[i].a.x != edges[i].b.x) | (edges[i].a.z != edges[i].b.z))
                    output.push_back(i);
        }
    }
    return output;
}

/**
 * @brief sign returns sign of value
 * @param v is value
 * @return one with sign of value
 */
int sign(int v) {
    if (v > 0)
        return 1;
    else
        return -1;
}

/**
 * @brief sqr squares a number
 * @param a is number to square
 * @return squared number
 */
float sqr(float a) {
    return a * a;
}
