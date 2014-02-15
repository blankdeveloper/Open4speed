//----------------------------------------------------------------------------------------
/**
 * \file       math.cpp
 * \author     Vonasek Lubos
 * \date       2014/01/05
 * \brief      Common math utils used in program.
*/
//----------------------------------------------------------------------------------------

#include <math.h>
#include "utils/math.h"

/**
 * @brief absf gets absolute value of float number
 * @param a is input value
 * @return absolute value of number
 */
float absf(float n) {
    if (n >= 0)
        return n;
    else
        return -n;
}

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
 * @brief angle counts angle of edge
 * @param e is instance of edge
 * @return angle in radians
 */
float angle(edge e) {
    return angle(e.ax, e.az, e.bx, e.bz);
}

/**
 * @brief angle counts angle between two cars
 * @param a is instance of first car
 * @param b is instance of second car
 * @return angle in radians
 */
float angle(car* a, car* b) {
    return angle(a->x, a->y, b->x, b->y);
}

/**
 * @brief angle counts angle between car and of the end edge
 * @param e is instance of edge
 * @param c is instance of car
 * @return angle in radians
 */
float angle(edge e, car* c) {
    return angle(c->x, c->z, e.bx, e.bz);
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
 * @brief dist is distance between two points in 3D
 * @param x1 is first point x coordinate
 * @param y1 is first point y coordinate
 * @param z1 is first point z coordinate
 * @param x2 is second point x coordinate
 * @param y2 is second point y coordinate
 * @param z2 is second point z coordinate
 * @return distance in meters
 */
float dist(float x1, float y1, float z1, float x2, float y2, float z2) {
    return sqrt(sqr(x2 - x1) + sqr(y2 - y1) + sqr(z2 - z1));
}

/**
 * @brief distance counts distance between two cars
 * @param a is instance of first car
 * @param b is instance of second car
 * @return distance in float
 */
float distance(car* a, car* b) {
    return distance(a->x, a->y, b->x, b->y);
}

/**
 * @brief distance counts distance between two cars in future
 * @param a is instance of first car
 * @param b is instance of second car
 * @param time is time in future(0=present)
 * @return distance in float
 */
float distance(car* a, car* b, float time) {
    float nax = a->x + sin(a->rot * 3.14 / 180) * time;
    float nay = a->y + cos(a->rot * 3.14 / 180) * time;
    float nbx = b->x + sin(b->rot * 3.14 / 180) * time;
    float nby = b->y + cos(b->rot * 3.14 / 180) * time;

    return distance(nax, nay, nbx, nby);
}

/**
 * @brief distance counts distance between car and of the end edge
 * @param e is instance of edge
 * @param c is instance of car
 * @return distance in float
 */
float distance(edge e, car* c) {
    return distance(c->x, c->z, e.bx, e.bz);
}

/**
 * @brief gap counts gap between car and of the end edge
 * @param e is instance of edge
 * @param c is instance of car
 * @return gap in radians
 */
float gap(edge e, car* c) {
    float direction = angle(e, c) * 180 / 3.14;
    float gap = c->rot - direction - 180;
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
 * @brief gap counts gap between two cars
 * @param a is instance of first car
 * @param b is instance of second car
 * @return gap in radians
 */
float gap(car* a, car* b) {
    float gap = a->rot - angle(a, b) - 180;
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
 * @brief gap counts gap between two cars in future
 * @param a is instance of first car
 * @param b is instance of second car
 * @param time is time in future(0=present)
 * @return gap in radians
 */
float gap(car* a, car* b, float time) {
    float nax = a->x - sin(a->rot * 3.14 / 180) * time * a->speed;
    float nay = a->y - cos(a->rot * 3.14 / 180) * time * a->speed;
    float nbx = b->x - sin(b->rot * 3.14 / 180) * time * b->speed;
    float nby = b->y - cos(b->rot * 3.14 / 180) * time * b->speed;

    float an = angle(nax, nay, nbx, nby);
    float gap = a->rot - an - 180;
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
    if ((a.ax == b.bx) & (a.az == b.bz))
        if ((b.ax == a.bx) & (b.az == a.bz))
            return true;
    if ((a.ax == b.ax) & (a.az == b.az))
        if ((b.bx == a.bx) & (b.bz == a.bz))
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
std::vector<int>* nextEdge(std::vector<edge> *edges, edge e) {
    std::vector<int> *output = new std::vector<int>();

    /// skip logical edges
    for (unsigned int i = 0; i < edges->size(); i++) {
        if (((*edges)[i].ax == e.bx) && ((*edges)[i].az == e.bz)) {
            if (e.by > (*edges)[i].ay) {
                e.by = (*edges)[i].ay;
            }
        }
    }

    /// add possible edges
    for (unsigned int i = 0; i < edges->size(); i++) {
        if (((*edges)[i].ax == e.bx) & ((*edges)[i].ay == e.by) & ((*edges)[i].az == e.bz)) {
            if (!isSame((*edges)[i],e))
                if (((*edges)[i].ax != (*edges)[i].bx) | ((*edges)[i].az != (*edges)[i].bz))
                    output->push_back(i);
        }
    }
    return output;
}

/**
 * @brief sidemoveEdge moves edge into side
 * @param e is edge to move
 * @param amount is amount of movement
 * @return moved edge
 */
edge sidemoveEdge(edge e, float amount) {
    float a = angle(e) + 1.57;
    edge o = *(new edge());
    o.ax = e.ax + sin(a) * amount;
    o.ay = e.ay;
    o.az = e.az + cos(a) * amount;
    o.bx = e.bx + sin(a) * amount;
    o.by = e.by;
    o.bz = e.bz + cos(a) * amount;
    return o;
}

/**
 * @brief sqr squares a number
 * @param a is number to square
 * @return squared number
 */
float sqr(float a) {
    return a * a;
}
