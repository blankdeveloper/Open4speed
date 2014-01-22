//----------------------------------------------------------------------------------------
/**
 * \file       math.h
 * \author     Vonasek Lubos
 * \date       2014/01/05
 * \brief      Common math utils used in program.
*/
//----------------------------------------------------------------------------------------

#ifndef MATH_H
#define MATH_H

#include "stdafx.h"

/**
 * @brief absf gets absolute value of float number
 * @param a is input value
 * @return absolute value of number
 */
float absf(float a);

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
 * @brief angle counts angle of edge
 * @param e is instance of edge
 * @return angle in radians
 */
float angle(edge e);

/**
 * @brief angle counts angle between two cars
 * @param a is instance of first car
 * @param b is instance of second car
 * @return angle in radians
 */
float angle(car* a, car* b);

/**
 * @brief angle counts angle between car and of the end edge
 * @param e is instance of edge
 * @param c is instance of car
 * @return angle in radians
 */
float angle(edge e, car* c);

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
 * @brief distance counts distance between two cars
 * @param a is instance of first car
 * @param b is instance of second car
 * @return distance in float
 */
float distance(car* a, car* b);

/**
 * @brief distance counts distance between two cars in future
 * @param a is instance of first car
 * @param b is instance of second car
 * @param time is time in future(0=present)
 * @return distance in float
 */
float distance(car* a, car* b, float time);

/**
 * @brief distance counts distance between car and of the end edge
 * @param e is instance of edge
 * @param c is instance of car
 * @return distance in float
 */
float distance(edge e, car* c);

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
float dist(float x1, float y1, float z1, float x2, float y2, float z2);

/**
 * @brief gap counts gap between car and of the end edge
 * @param e is instance of edge
 * @param c is instance of car
 * @return gap in radians
 */
float gap(edge e, car* c);

/**
 * @brief gap counts gap between two cars
 * @param a is instance of first car
 * @param b is instance of second car
 * @return gap in radians
 */
float gap(car* a, car* b);

/**
 * @brief gap counts gap between two cars in future
 * @param a is instance of first car
 * @param b is instance of second car
 * @param time is time in future(0=present)
 * @return gap in radians
 */
float gap(car* a, car* b, float time);

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
 * @brief max get max value of input
 * @param a is first value
 * @param b is second value
 * @return bigger value
 */
int max(int a, int b);

/**
 * @brief min get min value of input
 * @param a is first value
 * @param b is second value
 * @return smaller value
 */
int min(int a, int b);

/**
 * @brief nextEdge gets all possible indexes of next edges
 * @param edges is track curve instance
 * @param e is current edge
 * @return indicies as vector of int
 */
std::vector<int>* nextEdge(std::vector<edge> *edges, edge e);

/**
 * @brief sidemoveEdge moves edge into side
 * @param e is edge to move
 * @param amount is amount of movement
 * @return moved edge
 */
edge sidemoveEdge(edge e, float amount);

/**
 * @brief sqr squares a number
 * @param a is number to square
 * @return squared number
 */
float sqr(float a);

#endif // MATH_H
