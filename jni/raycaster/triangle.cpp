#include <glm/gtx/intersect.hpp>
#include <string.h>
#include "loaders/pngloader.h"
#include "raycaster/triangle.h"
#include "raycaster/utils.h"
#include "utils/io.h"
#include "common.h"

triangle* lastIntersectedTriangle = 0;
triangle* lastIntersectedTriangle2 = 0;

float step = 1 / 255.0f;

triangle::triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c,
         glm::vec2 aID, glm::vec2 bID, glm::vec2 cID,
         glm::vec3 na, glm::vec3 nb, glm::vec3 nc,
         glm::vec2 ta, glm::vec2 tb, glm::vec2 tc,
         int lmIndex, long tIndex, Texture* txt, Texture* txtmap) {
    this->a = a;
    this->b = b;
    this->c = c;
    this->txt = txt;
    this->txtmap = txtmap;
    this->aID = glm::ivec2((int)(aID.x * rttsize + 0.5f), (int)(aID.y * rttsize + 0.5f));
    this->bID = glm::ivec2((int)(bID.x * rttsize + 0.5f), (int)(bID.y * rttsize + 0.5f));
    this->cID = glm::ivec2((int)(cID.x * rttsize + 0.5f), (int)(cID.y * rttsize + 0.5f));
    this->na = na;
    this->nb = nb;
    this->nc = nc;
    this->ta = ta;
    this->tb = tb;
    this->tc = tc;
    this->lmIndex = lmIndex;
    this->tIndex = tIndex;
    lastTestID = -1;
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

    points.push_back({vert, t, n, c});
}

PLP* triangle::getPointLight(glm::vec3 p) {
    float area = triangleArea(a, b, c);
    float lena = triangleArea(p, b, c) / area;
    float lenb = triangleArea(p, a, c) / area;
    float lenc = triangleArea(p, a, b) / area;
    glm::vec2 uv = ta * lena + tb * lenb + tc * lenc;
    uv = glm::mod(uv + 1024.0f, 1.0f);
    glm::ivec2 c = glm::ivec2(0.5f + uv.x * txtmap->width, 0.5f + uv.y * txtmap->height);
    PLP* plp = new PLP();
    plp->color = glm::vec4(txtmap->data[(c.y * txtmap->width + c.x) * 3 + 0],
                           txtmap->data[(c.y * txtmap->width + c.x) * 3 + 1],
                           txtmap->data[(c.y * txtmap->width + c.x) * 3 + 2], 0) / 255.0f;
    glm::vec3 normal = glm::normalize(na * lena + nb * lenb + nc * lenc);
    plp->pos = glm::vec4(p + normal * 0.1f, 1.0f);
    plp->dir = glm::vec4(-normal, 0.0f);
    plp->useable = ((plp->color.x > step) || (plp->color.y > step) || (plp->color.z > step));
    return plp;
}

glm::vec3 p = glm::vec3(0,0,0);

float ScalarTriple(glm::vec3 pq, glm::vec3 pc, glm::vec3 pb) {
    return glm::dot(pb, glm::cross(pq, pc));
}

bool triangle::isIntersectedByRay() {
    if (!glm::intersectLineTriangle(uniform->begin, uniform->raydir, a, b, c, p))
        return false;
    else if (txt != 0) {
        float area = triangleArea(a, b, c);
        float lena = triangleArea(p, b, c) / area;
        float lenb = triangleArea(p, a, c) / area;
        float lenc = triangleArea(p, a, b) / area;
        // find the uv corresponding to point p
        glm::vec2 uv = ta * lena + tb * lenb + tc * lenc;
        uv = glm::mod(uv + 1024.0f, 1.0f);
        glm::ivec2 c = glm::ivec2(uv.x * txt->width, uv.y * txt->height);
        if (txt->data[(c.y * txt->width + c.x) * 4 + 3] < 128)
            return false;
    }
    /*glm::vec3 pq = uend - ubegin;
    glm::vec3 pa = a - ubegin;
    glm::vec3 pb = b - ubegin;
    glm::vec3 pc = c - ubegin;
    if (ScalarTriple(pq, pc, pb) < 0.0f) return 0;
    if (ScalarTriple(pq, pa, pc) < 0.0f) return 0;
    if (ScalarTriple(pq, pb, pa) < 0.0f) return 0;*/

    if (lastIntersectedTriangle != 0)
        lastIntersectedTriangle2 = lastIntersectedTriangle;
    lastIntersectedTriangle = this;
    return true;
}
