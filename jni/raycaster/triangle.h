#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "renderers/opengl/gltexture.h"
#include "raycaster/utils.h"

extern int ignore1;     ///< index of triangle to ignore
extern int ignore2;     ///< index of triangle to ignore

class triangle
{
public:
    AABB reg;           ///< AABB of the object
    glm::vec3 a;        ///< 3D coord of first point
    glm::vec3 b;        ///< 3D coord of second point
    glm::vec3 c;        ///< 3D coord of third point
    glm::ivec2 aID;     ///< Lightmap coord of first point
    glm::ivec2 bID;     ///< Lightmap coord of second point
    glm::ivec2 cID;     ///< Lightmap coord of third point
    glm::vec3 na;       ///< Normal of first point
    glm::vec3 nb;       ///< Normal of second point
    glm::vec3 nc;       ///< Normal of third point
    glm::vec2 ta;       ///< Texture coord of first point
    glm::vec2 tb;       ///< Texture coord of second point
    glm::vec2 tc;       ///< Texture coord of third point
    int lmIndex;        ///< Index of lightmap
    long tIndex;        ///< Triangle index
    long testID;        ///< Index of last test
    Texture* txt;       ///< Triangle texture
    Texture* txtmap;    ///< Triangle map
    std::vector<Point*> points; ///< All triangle points

    /**
     * triangle is constructor of triangle
     * @param a is 3D coord of first point
     * @param b is 3D coord of second point
     * @param c is 3D coord of third point
     * @param aID is lightmap coord of first point
     * @param bID is lightmap coord of second point
     * @param cID is lightmap coord of third point
     * @param na is normal of first point
     * @param nb is normal of second point
     * @param nc is normal of third point
     * @param ta is texture coord of first point
     * @param tb is texture coord of second point
     * @param tc is texture coord of third point
     * @param lmIndex is index of lightmap
     * @param tIndex is triangle index
     * @param txt is triangle texture
     * @param txtmap is triangle map
     */
    triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c,
             glm::ivec2 aID, glm::ivec2 bID, glm::ivec2 cID,
             glm::vec3 na, glm::vec3 nb, glm::vec3 nc,
             glm::vec2 ta, glm::vec2 tb, glm::vec2 tc,
             int lmIndex, long tIndex, Texture* txt, Texture* txtmap);

    /**
     * addPointToAABB adds point into triangle AABB (resize AABB by point)
     * @param p is point to add
     */
    void addPointToAABB(glm::vec3 p);

    /**
     * addLMPoint adds point into triangle
     * @param u is u vector of point in triangle (from 0 to 255)
     * @param v is v vector of point in triangle (from 0 to 255)
     * @param t is position in lightmap
     */
    void addLMPoint(int u, int v, glm::ivec2 t);

    /**
     * countDistanceTo counts distance between two triangles (aproximate)
     * @param t is second triangle to count distance
     * @return distance between triangles
     */
    float countDistanceTo(triangle* t);

    /**
     * getPointLight gets point lightmap from map
     * @param p is 3D point in space
     * @return point light in PLP structure
     */
    PLP* getPointLight(glm::vec3 p);

    /**
     * ClosestPtPointTriangle count closest point on triangle from point
     * @param p is point to count
     * @return closest point on triangle
     */
    glm::vec3 ClosestPtPointTriangle(glm::vec3 p);

    /**
     * isIntersectedByRay tests triangle intersection with ray
     * @param raybegin is ray begin
     * @param rayend is ray end
     * @param testID is index of test
     * @return true if it is intersected
     */
    bool isIntersectedByRay(glm::vec3 raybegin, glm::vec3 rayend, long testID);
};

extern triangle* lastIntersectedTriangle;  ///< Last intersected triangle
extern triangle* lastIntersectedTriangle2; ///< Last intersected triangle

#endif // TRIANGLE_H
