#include "raycaster/utils.h"
#include "common.h"

const float EPSILON = 0.00001f;
timespec ts_start, ts_end;          ///< Time messurement

glm::vec3 ubegin;
glm::vec3 uend;
glm::vec3 uraydir;
long uignore1;
long uignore2;
long utestID;

glm::vec3 getColor(Point p) {


    //count light
    glm::vec3 lightDir = -uraydir;
    glm::vec3 N = glm::normalize(p.n);
    glm::vec3 L = glm::normalize(lightDir);
    //count effectivity(directionaly)
    float eff = glm::dot(glm::normalize(swizle(xrenderer->light.u_light_dir)), -L);
    //light cutoff
    if (eff >= xrenderer->light.u_light_cut) {
      //diffuse light
      glm::vec3 color = glm::max(glm::dot(N, L), 0.0f) * swizle(xrenderer->light.u_light_diffuse);
      //light attenuation
      float d = glm::abs(uraydir.z);
      glm::vec3 D = glm::vec3(1.0, d, d * d);
      color *= glm::pow(eff, xrenderer->light.u_light_spot_eff) / glm::dot(swizle(xrenderer->light.u_light_att), D);
      // add to previous lightmap
      return glm::clamp(color, 0, 1);
    } else {
      return glm::vec3(0, 0, 0);
    }
}

bool isBlack(glm::vec3 color) {
    if ((int)(color.x * 255.0f) > 0) return false;
    if ((int)(color.y * 255.0f) > 0) return false;
    if ((int)(color.z * 255.0f) > 0) return false;
    return true;
}

void setUniforms(glm::vec3 begin, glm::vec3 end, long ignore1, long ignore2, long testID) {
    ubegin = begin;
    uend = end;
    uignore1 = ignore1;
    uignore2 = ignore2;
    uraydir = end - begin;
    utestID = testID;
}


void startTimer() {
    fflush(stdout);
    clock_gettime(CLOCK_REALTIME, &ts_start);
}

void stopTimer() {
    clock_gettime(CLOCK_REALTIME, &ts_end);
    printf("%0.3fs\n", ts_end.tv_sec - ts_start.tv_sec + (ts_end.tv_nsec - ts_start.tv_nsec) * 0.000000001f);
}

glm::vec3 swizle(glm::vec4 v) {
    return glm::vec3(v.x, v.y, v.z);
}

bool TestAABBAABB(AABB* a, AABB* b) {
    // Exit with no intersection if separated along an axis
    if (a->max[0] < b->min[0] || a->min[0] > b->max[0]) return 0;
    if (a->max[1] < b->min[1] || a->min[1] > b->max[1]) return 0;
    if (a->max[2] < b->min[2] || a->min[2] > b->max[2]) return 0;
    // Overlapping on all axes means AABBs are intersecting
    return 1;
}

// Test if segment intersects AABB b
int TestSegmentAABB(AABB* b) {
    glm::vec3 c = (b->min + b->max) * 0.5f; // Box center-point
    glm::vec3 e = b->max - c; // Box halflength extents
    glm::vec3 m = (ubegin + uend) * 0.5f; // Segment midpoint
    glm::vec3 d = uend - m; // Segment halflength vector
    m = m - c; // Translate box and segment to origin
    // Try world coordinate axes as separating axes
    float adx = fabsf(d.x);
    if (fabsf(m.x) > e.x + adx) return 0;
    float ady = fabsf(d.y);
    if (fabsf(m.y) > e.y + ady) return 0;
    float adz = fabsf(d.z);
    if (fabsf(m.z) > e.z + adz) return 0;
    // Add in an epsilon term to counteract arithmetic errors when segment is
    // (near) parallel to a coordinate axis (see text for detail)
    adx += EPSILON; ady += EPSILON; adz += EPSILON;
    // Try cross products of segment direction vector with coordinate axes
    if (fabsf(m.y * d.z - m.z * d.y) > e.y * adz + e.z * ady) return 0;
    if (fabsf(m.z * d.x - m.x * d.z) > e.x * adz + e.z * adx) return 0;
    if (fabsf(m.x * d.y - m.y * d.x) > e.x * ady + e.y * adx) return 0;
    // No separating axis found; segment must be overlapping AABB
    return 1;
}

float triangleArea(glm::vec3 a, glm::vec3 b, glm::vec3 c) {
    return glm::length(glm::cross(c - a, c - b)) * 0.5f;
}

