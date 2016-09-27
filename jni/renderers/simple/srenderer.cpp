///----------------------------------------------------------------------------------------
/**
 * \file       simple.cpp
 * \author     Vonasek Lubos
 * \date       2015/11/29
 * \brief      Simple rendering 3D geometry
**/
///----------------------------------------------------------------------------------------


#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <sys/time.h>
#include "engine/switch.h"
#include "renderers/opengl/gles20.h"//just for RTT method
#include "renderers/simple/srenderer.h"

Color black = {0,0,0};

/**
 * @brief simple constructor
 */
srenderer::srenderer() {
    /// set default values
    for (int i = 0; i < 10; i++) {
        enable[i] = true;
    }
    aliasing = 1;
    camX = 0;
    camY = 0;
    camZ = 0;
    direction = 0;
    mode3D = 0;
    depthBuffer = 0;
    pixelBuffer = 0;
}

/**
 * @brief simple is constructor
 * @param width is width of viewport
 * @param height is height of viewport
 */
void srenderer::init(int width, int height) {
    //create viewport
    viewport_width = width;
    viewport_height = height;

    //create arrays
    if(depthBuffer)
        delete[] depthBuffer;
    if(pixelBuffer)
        delete[] pixelBuffer;
    depthBuffer = new double[width * height + 1];
    pixelBuffer = new Color[width * height + 1];
    clear();
}

/**
 * @brief renderer destructor
 */
srenderer::~srenderer() {
  if(depthBuffer)
      delete[] depthBuffer;
  if(pixelBuffer)
      delete[] pixelBuffer;
  depthBuffer = 0;
  pixelBuffer = 0;
}

/**
 * @brief clear clears depth and pixel buffers
 */
void srenderer::clear() {
    //clear arrays
    for (int i = 0; i < viewport_width * viewport_height + 1; i++) {
        depthBuffer[i] = 9999999999999999;
        //pixelBuffer[i] = black;
    }
}

/**
 * @brief renderModel renders model into scene
 * @param m is instance of model to render
 * @param center is model translation
 */
void srenderer::renderModel(model* m, glm::vec3 center) {

    for (unsigned int i = 0; i < m->models.size(); i++) {
        if (!m->models[i].texture2D->transparent && !m->models[i].touchable)
            if (enable[m->models[i].filter])
                renderSubModel(m, &m->models[i], center);
    }
    for (unsigned int i = 0; i < m->models.size(); i++) {
        if (m->models[i].texture2D->transparent && !m->models[i].touchable)
            if (enable[m->models[i].filter])
                renderSubModel(m, &m->models[i], center);
    }
}

/**
 * @brief renderSubModel renders model into scene
 * @param m is instance of model to render
 * @param center is model translation
 */
void srenderer::renderSubModel(model* mod, model3d *m, glm::vec3 center) {

    /// set model matrix
    glm::mat4x4 modelView;
    if (m->dynamic) {
        float mat[16];
        getPhysics()->getTransform(m->dynamicID, mat);
        m->x = mat[12];
        m->y = mat[13];
        m->z = mat[14];
        float w = m->reg.max.x - m->reg.min.x;
        float a = m->reg.max.y - m->reg.min.y;
        float h = m->reg.max.z - m->reg.min.z;
        glm::mat4x4 translation(
            1,0,0,0,
            0,1,0,0,
            0,0,1,0,
            -w/2 + center.x, -a/2 + center.y, -h/2 + center.z, 1
        );
        glm::mat4x4 dynamic(
            mat[0],mat[1],mat[2],mat[3],
            mat[4],mat[5],mat[6],mat[7],
            mat[8],mat[9],mat[10],mat[11],
            mat[12],mat[13],mat[14],mat[15]
        );
        modelMat = dynamic * translation;
        modelView = view_matrix * modelMat;
    } else {
        glm::mat4x4 translation(
            1,0,0,0,
            0,1,0,0,
            0,0,1,0,
            m->reg.min.x + center.x, m->reg.min.y + center.y, m->reg.min.z + center.z, 1
        );
        modelMat = matrix_result * translation;
        modelView = view_matrix * modelMat;
    }
    matrix = proj_matrix * modelView;
    //ortho mode
    float w = 38.4f;
    float h = 25.6f;
    glm::mat4 ortho = glm::ortho(-w, w, -h, h);
    matrix = ortho * modelView;

    // apply texture
    m->texture2D->apply();
    texture = (stexture*)(m->texture2D);

    /// standart vertices
    triangles(m->vertices, m->coords, 0, m->count);
}

/**
 * @brief rtt enables rendering into FBO which makes posible to do reflections
 * @param enable is true to start drawing, false to render on screen
 */
void srenderer::rtt(bool enable) {
    struct timeval tv;
    gettimeofday(&tv, 0);
    if(enable) {
        clear();
        timestamp = (unsigned int)(tv.tv_sec * 1000 + tv.tv_usec / 1000);
        return;
    }
    glDisable(GL_DEPTH_TEST);
    if(pixelBuffer)
      glDrawPixels( viewport_width, viewport_height, GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)pixelBuffer );
    printf("Software rendering: %dms\n", (unsigned int)(tv.tv_sec * 1000 + tv.tv_usec / 1000) - timestamp);
}

/**
 * RENDERING PRIMITIVES
 */

/**
  * @brief line makes markers for filling triangle
  * @param x1 is line start position x
  * @param y1 is line start position y
  * @param x2 is line end position x
  * @param y2 is line end position y
  * @param fillCache is cache to be used
  * @return true if line was processed
  */
bool srenderer::line(int x1, int y1, int x2, int y2, glm::dvec3 z1, glm::dvec3 z2, std::pair<int, glm::dvec3>* fillCache) {

    //Liang & Barsky clipping (only top-bottom)
    int h = y2 - y1;
    double t1 = 0, t2 = 1;
    if (test(-h, y1, t1, t2) && test(h, viewport_height - 1 - y1, t1, t2) ) {
        glm::dvec3 z;
        std::pair<int, glm::dvec3> v;
        int c0, c1, xp0, xp1, yp0, yp1, y, p, w;
        bool wp, hp;

        //clip line
        if (t1 > 0) {
            w = x2 - x1;
            z = z2 - z1;
            x1 += t1 * w;
            y1 += t1 * h;
            z1 += t1 * z;
        } else
            t1 = 0;
        if (t2 < 1) {
            w = x2 - x1;
            z = z2 - z1;
            t2 -= t1;
            x2 = x1 + t2 * w;
            y2 = y1 + t2 * h;
            z2 = z1 + t2 * z;
        }

        //count new line dimensions
        wp = (x2 >= x1) ? true : false;
        w = wp ? x2 - x1 : x1 - x2;
        hp = (y2 >= y1) ? true : false;
        h = hp ? y2 - y1 : y1 - y2;

        //line in x axis nearby
        if (w > h) {
            //direction from left to right
            xp0 = wp ? 1 : -1;
            yp0 = 0;

            //direction from top to bottom
            xp1 = wp ? 1 : -1;
            yp1 = hp ? 1 : -1;

        //line in y axis nearby
        } else {
            //direction from top to bottom
            xp0 = 0;
            yp0 = hp ? 1 : -1;

            //direction from left to right
            xp1 = wp ? 1 : -1;
            yp1 = hp ? 1 : -1;

            //apply line length
            y = w;
            w = h;
            h = y;
        }

        //count z coordinate step
        z = (z2 - z1) / (double)w;

        //Bresenham's algorithm
        c0 = h + h;
        p = c0 - w;
        c1 = p - w;
        y = y1;
        v.first = x1;
        v.second = z1;
        fillCache[y] = v;
        for (w--; w >= 0; w--) {

            //interpolate
            if (p < 0) {
                p += c0;
                x1 += xp0;
                y1 += yp0;
            } else {
                p += c1;
                x1 += xp1;
                y1 += yp1;
            }
            z1 += z;

            //write cache info
            if (wp || (y != y1)) {
                y = y1;
                v.first = x1;
                v.second = z1;
                fillCache[y] = v;
            }
        }
        return true;
    }
    return false;
}

/**
 * @brief test is Liang & Barsky clipping
 */
bool srenderer::test(double p, double q, double &t1, double &t2) {
    //negative cutting
    if (p < 0) {
        double t = q/p;

        //cut nothing
        if (t > t2)
            return false;
        //cut the first coordinate
        else if (t > t1)
            t1 = t;

    //positive cutting
    } else if (p > 0) {
        double t = q/p;

        //cut nothing
        if (t < t1)
            return false;
        //cut the second coordinate
        else if (t < t2)
            t2 = t;

    //line is right to left(or bottom to top)
    } else if (q < 0)
        return false;
    return true;
}

void srenderer::triangles(float* vertices, float* coords, int offset, int size) {
    int v = offset * 9;
    int t = offset * 6;
    int ab, ac, bc, step, x, x1, x2, y, min, mem, memy, max;
    double t1, t2;
    glm::vec4 a, b, c;
    glm::dvec3 az, bz, cz, z, z1, z2;

    std::pair<int, glm::dvec3> fillCache1[viewport_height + 1];
    std::pair<int, glm::dvec3> fillCache2[viewport_height + 1];

    for (int i = offset; i < size; i++, v += 9, t += 6) {
        //get vertices
        a = glm::vec4(vertices[v + 0], vertices[v + 1], vertices[v + 2], 1.0f);
        b = glm::vec4(vertices[v + 3], vertices[v + 4], vertices[v + 5], 1.0f);
        c = glm::vec4(vertices[v + 6], vertices[v + 7], vertices[v + 8], 1.0f);
        //transfer into screen coordinates from -1 to 1
        a = matrix * a;
        b = matrix * b;
        c = matrix * c;
        //scale into screen dimensions
        a.x = (a.x / fabs(a.w) + 1.0f) * 0.5f * viewport_width;
        a.y = (a.y / fabs(a.w) + 1.0f) * 0.5f * viewport_height;
        b.x = (b.x / fabs(b.w) + 1.0f) * 0.5f * viewport_width;
        b.y = (b.y / fabs(b.w) + 1.0f) * 0.5f * viewport_height;
        c.x = (c.x / fabs(c.w) + 1.0f) * 0.5f * viewport_width;
        c.y = (c.y / fabs(c.w) + 1.0f) * 0.5f * viewport_height;
        //get texture coordinates and depth for interpolation
        az = glm::dvec3(coords[t + 0], coords[t + 1], a.z);
        bz = glm::dvec3(coords[t + 2], coords[t + 3], b.z);
        cz = glm::dvec3(coords[t + 4], coords[t + 5], c.z);

        //create markers for filling
        ab = glm::abs(a.y - b.y);
        ac = glm::abs(a.y - c.y);
        bc = glm::abs(b.y - c.y);
        if ((ab >= ac) && (ab >= bc)) {
            line(a.x, a.y, b.x, b.y, az, bz, fillCache1);
            line(a.x, a.y, c.x, c.y, az, cz, fillCache2);
            line(b.x, b.y, c.x, c.y, bz, cz, fillCache2);
            min = glm::max(0.0f, glm::min(a.y, b.y));
            max = glm::min(glm::max(a.y, b.y), viewport_height - 1.0f);
        } else if ((ac >= ab) && (ac >= bc)) {
            line(a.x, a.y, c.x, c.y, az, cz, fillCache1);
            line(a.x, a.y, b.x, b.y, az, bz, fillCache2);
            line(b.x, b.y, c.x, c.y, bz, cz, fillCache2);
            min = glm::max(0.0f, glm::min(a.y, c.y));
            max = glm::min(glm::max(a.y, c.y), viewport_height - 1.0f);
        } else if ((bc >= ab) && (bc >= ac)) {
            line(b.x, b.y, c.x, c.y, bz, cz, fillCache1);
            line(a.x, a.y, b.x, b.y, az, bz, fillCache2);
            line(a.x, a.y, c.x, c.y, az, cz, fillCache2);
            min = glm::max(0.0f, glm::min(b.y, c.y));
            max = glm::min(glm::max(b.y, c.y), viewport_height - 1.0f);
        }

        //fill triangle
        memy = min * viewport_width;
        for (y = min; y <= max; y++) {
            x1 = fillCache1[y].first;
            x2 = fillCache2[y].first;
            z1 = fillCache1[y].second;
            z2 = fillCache2[y].second;

            //Liang & Barsky clipping
            t1 = 0;
            t2 = 1;
            x = x2 - x1;
            if (test(-x, x1, t1, t2) && test(x, viewport_width - 1 - x1, t1, t2)) {

                //clip line
                z = z2 - z1;
                if (t1 > 0) {
                    x1 += t1 * x;
                    z1 += t1 * z;
                } else
                    t1 = 0;
                if (t2 < 1) {
                    t2 -= t1;
                    x2 = x1 + t2 * x;
                    z2 = z1 + t2 * z;
                }

                //filling algorithm initialize
                x = glm::abs(x2 - x1);
                step = (x2 >= x1) ? 1 : -1;
                z = (z2 - z1) / (double)x;
                mem = x1 + memy;
                for (; x >= 0; x--) {

                    //write pixel into memory and depth test
                    if ((0 < z1.z) && (depthBuffer[mem] > z1.z))
                        if(texture->setPixel(pixelBuffer, mem, z1.x, z1.y))
                            depthBuffer[mem] = z1.z;

                    //interpolate
                    mem += step;
                    z1 += z;
                }
            }
            memy += viewport_width;
        }
    }
}

/*
 * MATRICES
 */

/**
 * @brief lookAt implements GLUlookAt
 * @param eye is eye vector
 * @param center is camera center
 * @param up is up vector
 */
void srenderer::lookAt(glm::vec3 eye, glm::vec3 center, glm::vec3 up) {

    camX = eye.x;
    camY = eye.y;
    camZ = eye.z;
    direction = angle(center, eye);
    view_matrix = glm::lookAt(eye, center, up);
    matrix_result = glm::mat4x4(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);
}

/**
 * @brief perspective implements GLUPerspective
 * @param fovy is fov angle
 * @param aspect is screen aspect ration
 * @param zNear is near cutting plate
 * @param zFar is far cutting plane
 */
void srenderer::perspective(float fovy, float aspect, float zNear, float zFar) {
    while(!matrixBuffer.empty()) {
        matrixBuffer.pop();
    }
    proj_matrix = glm::perspective((float)(fovy * M_PI / 180.0f), aspect, zNear,zFar);
}

/**
 * @brief multMatrix multiplies with matrix
 * @param matrix is 4x4 matrix in OpenGL format
 */
void srenderer::multMatrix(float* matrix) {
  matrix_result *= glm::mat4x4(
      matrix[0], matrix[1], matrix[2], matrix[3],
      matrix[4], matrix[5], matrix[6], matrix[7],
      matrix[8], matrix[9], matrix[10], matrix[11],
      matrix[12], matrix[13], matrix[14], matrix[15]
  );
}

/**
 * @brief popMatrix pops matrix from stack
 */
void srenderer::popMatrix() {
  /// popping matrix from stack
  matrix_result = (glm::mat4x4)matrixBuffer.top();
  matrixBuffer.pop();
}

/**
 * @brief pushMatrix pushs current matrix to stack
 */
void srenderer::pushMatrix() {
  /// push matrix m to stack
  matrixBuffer.push(matrix_result);
}

/**
 * @brief rotateX rotate around X axis
 * @param value is angle
 */
void srenderer::rotateX(float value) {
  float radian = value * M_PI / 180;
  /// rotation matrix for 2D transformations (around Z axis)
  glm::mat4x4 rotation(
      1,0,0,0,
      0,cosf(radian),-sinf(radian),0,
      0,sinf(radian),cosf(radian),0,
      0,0,0,1
  );
  /// mult current maxtrix
  matrix_result *= rotation;
}

/**
 * @brief rotateX rotate around Y axis
 * @param value is angle
 */
void srenderer::rotateY(float value) {
  float radian = value * M_PI / 180;
  /// rotation matrix for 2D transformations (around Z axis)
  glm::mat4x4 rotation(
      cosf(radian),0,-sinf(radian),0,
      0,1,0,0,
      sinf(radian),0,cosf(radian),0,
      0,0,0,1
  );
  /// mult current maxtrix
  matrix_result *= rotation;
}

/**
 * @brief rotateX rotate around Z axis
 * @param value is angle
 */
void srenderer::rotateZ(float value) {
  float radian = value * M_PI / 180;
  /// rotation matrix for 2D transformations (around Z axis)
  glm::mat4x4 rotation(
      cosf(radian),sinf(radian),0,0,
      -sinf(radian),cosf(radian),0,0,
      0,0,1,0,
      0,0,0,1
  );
  /// mult current maxtrix
  matrix_result *= rotation;
}

/**
 * @brief scale scales current matrix
 * @param value is amount of scale(1 to keep current)
 */
void srenderer::scale(float value) {
  /// scale matrix
  glm::mat4x4 scale(
      value,0,0,0,
      0,value,0,0,
      0,0,value,0,
      0,0,0,1
  );
  /// mult current maxtrix
  matrix_result *= scale;
}

/**
 * @brief translate translates object
 * @param x is translate coordinate
 * @param y is translate coordinate
 * @param z is translate coordinate
 */
void srenderer::translate(float x, float y, float z) {
  /// transformation matrix
  glm::mat4x4 translation(
      1,0,0,0,
      0,1,0,0,
      0,0,1,0,
      x,y,z,1
  );
  /// mult current matrix
  matrix_result *= translation;
}

