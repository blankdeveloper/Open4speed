#include "raycaster/utils.h"
#include "common.h"

const float EPSILON = 0.00001f;     ///< Small number used for float comparation
timespec ts_start, ts_end;          ///< Time messurement
float cstep = 1 / 255.0f;           ///< Minimal color step

/**
 * countLightMaxDistance counts maximal distance of light
 * @param att is quadratic attenuation
 * @return distance
 */
float countLightMaxDistance(float att) {
    float maxColor = xrenderer->light.u_light_diffuse.x;
    maxColor = glm::max(maxColor, xrenderer->light.u_light_diffuse.y);
    maxColor = glm::max(maxColor, xrenderer->light.u_light_diffuse.z);
    return glm::sqrt(255.0f * maxColor / att);
}

/**
 * getColor gets color of 3D point for current point ray
 * @param p is 3D point
 * @param raybegin is ray begin
 * @param rayend is ray end
 * @return RGB color + intensity in A channel
 */
glm::vec4 getColor(Point *p, glm::vec3 raybegin, glm::vec3 rayend) {

    //count effectivity
    float sEff = 1;
    glm::vec3 raydir = rayend - raybegin;
    glm::vec3 L = glm::normalize(-raydir);
    if (xrenderer->light.u_light_cut != -2)
        sEff = glm::dot(glm::normalize(-swizle(xrenderer->light.u_light_dir)), -L);

    //light cutoff
    if (sEff >= xrenderer->light.u_light_cut) {
      //diffuse light
      glm::vec4 color = glm::max(glm::dot(p->n, L), 0.0f) * xrenderer->light.u_light_diffuse;
      color.w = 1;
      //light attenuation
      float d = glm::length(raydir);
      color *= pow(sEff, xrenderer->light.u_light_spot_eff) / glm::dot(xrenderer->light.u_light_att, glm::vec4(1.0, d, d * d, 0));
      // add to previous lightmap
      color = glm::clamp(color, glm::vec4(0, 0, 0, 1), glm::vec4(1, 1, 1, 1));

      if ((color.x < cstep) && (color.y < cstep) && (color.z < cstep))
          return glm::vec4(0, 0, 0, 0);
      else
          return glm::clamp(color, glm::vec4(0, 0, 0, 1), glm::vec4(1, 1, 1, 1));

    } else {
      return glm::vec4(0, 0, 0, 0);
    }
}

/**
 * sign is method for PointInTriangle check
 * @param p1 is pt vector
 * @param p2 is vx1 vector
 * @param p3 is vx2 vector
 * @return float value
 */
float sign(glm::ivec3 p1, glm::ivec3 p2, glm::ivec3 p3)
{
  return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

/**
 * PointInTriangle check if 2D point is in triangle
 * @param pt is point to check
 * @param v1 is first vertex of triangle
 * @param v2 is second vertex of triangle
 * @param v3 is triangle vertex of triangle
 * @return true if it is inside
 */
bool PointInTriangle(glm::ivec3 pt, glm::ivec3 v1, glm::ivec3 v2, glm::ivec3 v3)
{
  bool b1 = sign(pt, v1, v2) < 0;
  bool b2 = sign(pt, v2, v3) < 0;
  bool b3 = sign(pt, v3, v1) < 0;

  return ((b1 == b2) && (b2 == b3));
}

/**
 * startTimer starts timer
 */
void startTimer() {
    fflush(stdout);
    clock_gettime(CLOCK_REALTIME, &ts_start);
}

/**
 * stopTimer stops timer
 */
void stopTimer() {
    clock_gettime(CLOCK_REALTIME, &ts_end);
    printf("%0.3fs\n", ts_end.tv_sec - ts_start.tv_sec + (ts_end.tv_nsec - ts_start.tv_nsec) * 0.000000001f);
}

/**
 * swizle swizles vec4 to vec3
 * @param v is vec4
 * @return vec3
 */
glm::vec3 swizle(glm::vec4 v) {
    return glm::vec3(v.x, v.y, v.z);
}

/**
 * TestAABBAABB test if two AABBs intersecting
 * @param a is first AABB
 * @param b is second AABB
 * @return true if it is intersecting
 */
bool TestAABBAABB(AABB* a, AABB* b) {
    // Exit with no intersection if separated along an axis
    if (a->max[0] < b->min[0] || a->min[0] > b->max[0]) return 0;
    if (a->max[1] < b->min[1] || a->min[1] > b->max[1]) return 0;
    if (a->max[2] < b->min[2] || a->min[2] > b->max[2]) return 0;
    // Overlapping on all axes means AABBs are intersecting
    return 1;
}

/**
 * TestSegmentAABB tests if segment intersects AABB b
 * @param b is AABB to test
 * @param raybegin is ray begin
 * @param rayend is ray end
 * @return 1 if segment intersects AABB
 */
int TestSegmentAABB(AABB* b, glm::vec3 raybegin, glm::vec3 rayend) {
    glm::vec3 c = (b->min + b->max) * 0.5f; // Box center-point
    glm::vec3 e = b->max - c; // Box halflength extents
    glm::vec3 m = (raybegin + rayend) * 0.5f; // Segment midpoint
    glm::vec3 d = rayend - m; // Segment halflength vector
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

/**
 * triangleArea counts area of triangle
 * @param a is first point of triangle
 * @param b is second point of triangle
 * @param c is third point of triangle
 * @return area of triangle
 */
float triangleArea(glm::vec3 a, glm::vec3 b, glm::vec3 c) {
    return glm::length(glm::cross(c - a, c - b)) * 0.5f;
}

