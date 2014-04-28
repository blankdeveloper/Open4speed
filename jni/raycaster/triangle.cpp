#include <glm/gtx/intersect.hpp>
#include <string.h>
#include "loaders/pngloader.h"
#include "raycaster/triangle.h"
#include "raycaster/utils.h"
#include "utils/io.h"
#include "common.h"

triangle* lastIntersectedTriangle = 0;  ///< Last intersected triangle
triangle* lastIntersectedTriangle2 = 0; ///< Last intersected triangle

float step = 1 / 255.0f; ///< Minimal color step
int ignore1 = 0;         ///< index of triangle to ignore
int ignore2 = 0;         ///< index of triangle to ignore

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
triangle::triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c,
         glm::ivec2 aID, glm::ivec2 bID, glm::ivec2 cID,
         glm::vec3 na, glm::vec3 nb, glm::vec3 nc,
         glm::vec2 ta, glm::vec2 tb, glm::vec2 tc,
         int lmIndex, long tIndex, Texture* txt, Texture* txtmap) {
    this->a = a;
    this->b = b;
    this->c = c;
    this->txt = txt;
    this->txtmap = txtmap;
    this->aID = aID;
    this->bID = bID;
    this->cID = cID;
    this->na = na;
    this->nb = nb;
    this->nc = nc;
    this->ta = ta;
    this->tb = tb;
    this->tc = tc;
    this->lmIndex = lmIndex;
    this->tIndex = tIndex;
    reg.min.x = 9999999;
    reg.min.y = 9999999;
    reg.min.z = 9999999;
    reg.max.x = -9999999;
    reg.max.y = -9999999;
    reg.max.z = -9999999;
    addPointToAABB(a);
    addPointToAABB(b);
    addPointToAABB(c);
}

/**
 * addPointToAABB adds point into triangle AABB (resize AABB by point)
 * @param p is point to add
 */
void triangle::addPointToAABB(glm::vec3 p) {
    if (reg.min.x > p.x)
        reg.min.x = p.x;
    if (reg.min.y > p.y)
        reg.min.y = p.y;
    if (reg.min.z > p.z)
        reg.min.z = p.z;

    if (reg.max.x < p.x)
        reg.max.x = p.x;
    if (reg.max.y < p.y)
        reg.max.y = p.y;
    if (reg.max.z < p.z)
        reg.max.z = p.z;
}

/**
 * addLMPoint adds point into triangle
 * @param u is u vector of point in triangle (from 0 to 255)
 * @param v is v vector of point in triangle (from 0 to 255)
 * @param t is position in lightmap
 */
void triangle::addLMPoint(int u, int v, glm::ivec2 t) {
    glm::vec3 A = glm::vec3(0, 0, 1);
    glm::vec3 B = glm::vec3(1, 0, 1);
    glm::vec3 C = glm::vec3(0, 1, 1);
    glm::vec3 P = glm::vec3(u / 255.0f, v / 255.0f, 1);
    float area = triangleArea(A, B, C);
    float lena = triangleArea(P, B, C) / area;
    float lenb = triangleArea(P, A, C) / area;
    float lenc = triangleArea(P, A, B) / area;

    glm::vec3 vert = a * lena + b * lenb + c * lenc;
    glm::vec3 n = glm::normalize(na * lena + nb * lenb + nc * lenc);
    glm::vec2 c = ta * lena + tb * lenb + tc * lenc;

    points.push_back(new Point{vert, t, n, c, glm::vec3(lena, lenb, lenc), glm::vec2(P.x, P.y)});
}

/**
 * countDistanceTo counts distance between two triangles (aproximate)
 * @param t is second triangle to count distance
 * @return distance between triangles
 */
float triangle::countDistanceTo(triangle* t) {
    float min = glm::length(t->a - a);
    min = glm::min(min, glm::length(t->a - b));
    min = glm::min(min, glm::length(t->a - c));
    min = glm::min(min, glm::length(t->b - a));
    min = glm::min(min, glm::length(t->b - b));
    min = glm::min(min, glm::length(t->b - c));
    min = glm::min(min, glm::length(t->c - a));
    min = glm::min(min, glm::length(t->c - b));
    min = glm::min(min, glm::length(t->c - c));
    return min;
}

/**
 * getPointLight gets point lightmap from map
 * @param p is 3D point in space
 * @return point light in PLP structure
 */
PLP* triangle::getPointLight(glm::vec3 p) {
    float area = triangleArea(a, b, c);
    float lena = triangleArea(p, b, c) / area;
    float lenb = triangleArea(p, a, c) / area;
    float lenc = triangleArea(p, a, b) / area;
    glm::vec2 uv = ta * lena + tb * lenb + tc * lenc;
    uv = glm::mod(uv + 1024.0f, 1.0f);
    glm::ivec2 tc = glm::ivec2(0.5f + uv.x * txtmap->width, 0.5f + uv.y * txtmap->height);
    PLP* plp = new PLP();
    plp->color = glm::vec4(txtmap->data[(tc.y * txtmap->width + tc.x) * 3 + 0],
                           txtmap->data[(tc.y * txtmap->width + tc.x) * 3 + 1],
                           txtmap->data[(tc.y * txtmap->width + tc.x) * 3 + 2], 0) / 255.0f;
    glm::vec3 normal = glm::normalize(na * lena + nb * lenb + nc * lenc);
    plp->pos = glm::vec4(p, 1.0f);
    plp->dir = glm::vec4(-normal, 0.0f);
    plp->useable = ((plp->color.x > step) || (plp->color.y > step) || (plp->color.z > step));
    return plp;
}

/**
 * ClosestPtPointTriangle count closest point on triangle from point
 * @param p is point to count
 * @return closest point on triangle
 */
glm::vec3 triangle::ClosestPtPointTriangle(glm::vec3 p) {
    glm::vec3 ab = b - a;
    glm::vec3 ac = c - a;
    glm::vec3 bc = c - b;
    // Compute parametric position s for projection P’ of P on AB,
    // P’ = A + s*AB, s = snom/(snom+sdenom)
    float snom = glm::dot(p - a, ab);
    float sdenom = glm::dot(p - b, a - b);
    // Compute parametric position t for projection P’ of P on AC,
    // P’ = A + t*AC, s = tnom/(tnom+tdenom)
    float tnom = glm::dot(p - a, ac);
    float tdenom = glm::dot(p - c, a - c);
    if (snom <= 0.0f && tnom <= 0.0f) return a; // Vertex region early out
    // Compute parametric position u for projection P’ of P on BC,
    // P’ = B + u*BC, u = unom/(unom+udenom)
    float unom = glm::dot(p - b, bc);
    float udenom = glm::dot(p - c, b - c);
    if (sdenom <= 0.0f && unom <= 0.0f) return b; // Vertex region early out
    if (tdenom <= 0.0f && udenom <= 0.0f) return c; // Vertex region early out
    // P is outside (or on) AB if the triple scalar product [N PA PB] <= 0
    glm::vec3 n = glm::cross(b - a, c - a);
    float vc = glm::dot(n, glm::cross(a - p, b - p));
    // If P outside AB and within feature region of AB,
    // return projection of P onto AB
    if (vc <= 0.0f && snom >= 0.0f && sdenom >= 0.0f)
    return a + snom / (snom + sdenom) * ab;
    // P is outside (or on) BC if the triple scalar product [N PB PC] <= 0
    float va = glm::dot(n, glm::cross(b - p, c - p));
    // If P outside BC and within feature region of BC,
    // return projection of P onto BC
    if (va <= 0.0f && unom >= 0.0f && udenom >= 0.0f)
    return b + unom / (unom + udenom) * bc;
    // P is outside (or on) CA if the triple scalar product [N PC PA] <= 0
    float vb = glm::dot(n, glm::cross(c - p, a - p));
    // If P outside CA and within feature region of CA,
    // return projection of P onto CA
    if (vb <= 0.0f && tnom >= 0.0f && tdenom >= 0.0f)
    return a + tnom / (tnom + tdenom) * ac;
    // P must project inside face region. Compute Q using barycentric coordinates
    float u = va / (va + vb + vc);
    float v = vb / (va + vb + vc);
    float w = 1.0f - u - v; // = vc / (va + vb + vc)
    return u * a + v * b + w * c;
}

/**
 * isIntersectedByRay tests triangle intersection with ray
 * @param raybegin is ray begin
 * @param rayend is ray end
 * @param testID is index of test
 * @return true if it is intersected
 */
bool triangle::isIntersectedByRay(glm::vec3 raybegin, glm::vec3 rayend, long testID) {

    /// skip on test itself or it was already tested
    if ((tIndex == ignore1) || (tIndex == ignore2 || (this->testID == testID)))
        return false;

    /// get intersection by ray
    this->testID = testID;
    glm::vec3 pq = raybegin - rayend;
    glm::vec3 m = glm::cross(pq, rayend);
    float u = glm::dot(pq, glm::cross(c, b)) + glm::dot(m, c - b);
    float v = glm::dot(pq, glm::cross(a, c)) + glm::dot(m, a - c);
    float w = glm::dot(pq, glm::cross(b, a)) + glm::dot(m, b - a);
    if ((u < 0) || (v < 0) || (w < 0))
        return false;
    /// check if point is visible in texture
    else if (txt != 0) {
        // find the uv corresponding to point p
        float denom = 1.0f / (u + v + w);
        u *= denom;
        v *= denom;
        w *= denom;
        glm::vec2 uv = ta * u + tb * v + tc * w;
        uv = glm::mod(uv + 1024.0f, 1.0f);
        glm::ivec2 c = glm::ivec2(uv.x * txt->width, uv.y * txt->height);
        if (txt->data[(c.y * txt->width + c.x) * 4 + 3] < 128)
            return false;
    }

    /// self and neightbour intersection protection
    if ((glm::length(ClosestPtPointTriangle(raybegin) - raybegin) < 0.005f)
     || (glm::length(ClosestPtPointTriangle(rayend) - rayend) < 0.005f))
        return false;

    if (lastIntersectedTriangle != 0)
    {
        lastIntersectedTriangle2 = lastIntersectedTriangle;
        if (lastIntersectedTriangle2->tIndex != tIndex)
            lastIntersectedTriangle = this;
    } else
        lastIntersectedTriangle = this;
    return true;
}
