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
#include "renderers/simple/simple.h"

Color black = {0,0,0};
Color white = {255,255,255};

/**
 * @brief simple constructor
 */
simple::simple() {
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
    viewDistance = 1000;

    depthBuffer = 0;
    pixelBuffer = 0;
    fillCache1 = 0;
    fillCache2 = 0;
}

/**
 * @brief simple is constructor
 * @param width is width of viewport
 * @param height is height of viewport
 */
void simple::init(int width, int height) {
    //create viewport
    viewport_x = 0;
    viewport_y = 0;
    viewport_width = width;
    viewport_height = height;

    //create arrays
    if(depthBuffer)
        delete[] depthBuffer;
    if(pixelBuffer)
        delete[] pixelBuffer;
    if(fillCache1)
        delete[] fillCache1;
    if(fillCache2)
        delete[] fillCache2;
    depthBuffer = new double[width * height + 1];
    pixelBuffer = new Color[width * height + 1];
    fillCache1 = new std::pair<int, glm::dvec3>[height + 1];
    fillCache2 = new std::pair<int, glm::dvec3>[height + 1];
    clear();
}

/**
 * @brief renderer destructor
 */
simple::~simple() {
  if(depthBuffer)
      delete[] depthBuffer;
  if(pixelBuffer)
      delete[] pixelBuffer;
  if(fillCache1)
      delete[] fillCache1;
  if(fillCache2)
      delete[] fillCache2;
  depthBuffer = 0;
  pixelBuffer = 0;
  fillCache1 = 0;
  fillCache2 = 0;
}

/**
 * @brief clear clears depth and pixel buffers
 */
void simple::clear() {
    //clear arrays
    for (int i = 0; i < viewport_width * viewport_height + 1; i++) {
        depthBuffer[i] = viewDistance;
        //pixelBuffer[i] = black;
    }
}

/**
 * @brief renderModel renders model into scene
 * @param m is instance of model to render
 * @param physic is physical model instance
 * @param gamma is requested render gamma
 */
void simple::renderModel(model* m) {

    /// set culling info positions
    float avd = viewDistance / 200.0f;
    xm = (camX - m->aabb.min.x) / culling;
    xp = xm;
    ym = (camZ - m->aabb.min.z) / culling;
    yp = ym;
    for (float view = direction - M_PI * 0.75f; view <= direction + M_PI * 0.75f; view += M_PI * 0.25f)
    {
        if (xp < (camX - m->aabb.min.x) / culling + sin(view) * avd)
            xp = (camX - m->aabb.min.x) / culling + sin(view) * avd;
        if (xm > (camX - m->aabb.min.x) / culling + sin(view) * avd)
            xm = (camX - m->aabb.min.x) / culling + sin(view) * avd;
        if (yp < (camZ - m->aabb.min.z) / culling + cos(view) * avd)
            yp = (camZ - m->aabb.min.z) / culling + cos(view) * avd;
        if (ym > (camZ - m->aabb.min.z) / culling + cos(view) * avd)
            ym = (camZ - m->aabb.min.z) / culling + cos(view) * avd;
    }
    if (xm < 0)
        xm = 0;
    if (ym < 0)
        ym = 0;
    if (xp >= m->cutX)
        xp = m->cutX - 1;
    if (yp >= m->cutY)
        yp = m->cutY - 1;

    for (unsigned int i = 0; i < m->models.size(); i++) {
        if (!m->models[i].texture2D->transparent && !m->models[i].touchable)
            if (enable[m->models[i].filter])
                renderSubModel(m, &m->models[i]);
    }
    for (unsigned int i = 0; i < m->models.size(); i++) {
        if (m->models[i].texture2D->transparent && !m->models[i].touchable)
            if (enable[m->models[i].filter])
                renderSubModel(m, &m->models[i]);
    }
}

/**
 * @brief renderSubModel renders model into scene
 * @param m is instance of model to render
 */
void simple::renderSubModel(model* mod, model3d *m) {

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
            -w/2, -a/2, -h/2, 1
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
            m->reg.min.x, m->reg.min.y, m->reg.min.z,1
        );
        modelMat = matrix_result * translation;
        modelView = view_matrix * modelMat;
    }
    matrix = proj_matrix * modelView;

#ifdef USE_VBO
    printf("unsupported rendering models from GPU in software renderer\n");
    return;
#endif

    /// standart vertices
    if (mod->cutX * mod->cutY == 1) {
        triangles(m->vertices, 0, m->triangleCount[mod->cutX * mod->cutY]);
    }

    /// culled vertices
    else {
        for (int i = ym; i <= yp; i++) {
            int l = m->triangleCount[i * mod->cutX + xm];
            int r = m->triangleCount[i * mod->cutX + xp + 1];
            triangles(m->vertices, l, r);
        }
        int l = m->triangleCount[(mod->cutX - 1) * mod->cutY];
        int r = m->triangleCount[mod->cutX * mod->cutY];
        triangles(m->vertices, l, r);
    }
}

/**
 * @brief rtt enables rendering into FBO which makes posible to do reflections
 * @param enable is true to start drawing, false to render on screen
 */
void simple::rtt(bool enable) {
    struct timeval tv;
    gettimeofday(&tv, 0);
    if(enable) {
        clear();
        timestamp = (unsigned int)(tv.tv_sec * 1000 + tv.tv_usec / 1000);
        return;
    }
    glDisable(GL_DEPTH_TEST);
    glDrawPixels( viewport_width, viewport_height, GL_RGB, GL_UNSIGNED_BYTE, getBuffer() );
    printf("Software rendering: %dms\n", (unsigned int)(tv.tv_sec * 1000 + tv.tv_usec / 1000) - timestamp);
}

/**
 * RENDERING PRIMITIVES
 */

/**
  * @brief line draws line into pixel buffer
  * @param x1 is line start position x
  * @param y1 is line start position y
  * @param x2 is line end position x
  * @param y2 is line end position y
  * @param dm is draw mode to be used
  * @return true if line was drawed
  */
bool simple::line(int x1, int y1, int x2, int y2, glm::dvec3 z1, glm::dvec3 z2, DrawMode dm) {

    int a0, a1, b0, b1, c0, c1, d0, d1, h, p, w, mem;
    double t1, t2, dx, dy;
    bool wp, hp;
    glm::dvec3 dz, z;
    std::pair<int, glm::dvec3>* fillCache = dm == Marker1 ? fillCache1 : fillCache2;

    //Liang & Barsky clipping
    t1 = 0;
    t2 = 1;
    dx = x2 - x1;
    dy = y2 - y1;
    dz = z2 - z1;
    if ((dm != Normal || (test(-dx, x1, t1, t2) && test(dx, viewport_width - 1 - x1, t1, t2)))  &&
        test(-dy, y1, t1, t2) && test(dy, viewport_height - 1 - y1, t1, t2) ) {
        if (t1 > 0) {
            x1 += t1 * dx;
            y1 += t1 * dy;
            z1 += t1 * dz;
        } else
            t1 = 0;
        if (t2 < 1) {
            x2 = x1 + (t2 - t1) * dx;
            y2 = y1 + (t2 - t1) * dy;
            z2 = z1 + (t2 - t1) * dz;
        } else
            t2 = 1;

        //count line dimensions
        w = (int)(x2 - x1);
        h = (int)(y2 - y1);
        z = z2 - z1;

        //check if dimension is positive
        if (w >= 0) {
            wp = true;
        } else {
            wp = false;
            w = -w;
        }

        if (h >= 0) {
            hp = true;
        } else {
            hp = false;
            h = -h;
        }

        //line in x axis nearby
        if (w > h) {
            //count Bresenham's alg. variables
            c0 = 2 * h;
            p = c0 - w;
            c1 = p - w;

            //direction from left to right
            a0 = wp ? 1 : -1;
            b0 = 0;
            d0 = a0;

            //direction from top to bottom
            a1 = hp ? a0 + viewport_width : a0 - viewport_width;
            b1 = hp ? 1 : -1;
            d1 = a0;

        //line in y axis nearby
        } else {
            //count Bresenham's alg. variables
            c0 = 2 * w;
            p = c0 - h;
            c1 = p - h;

            //direction from top to bottom
            a0 = hp ? viewport_width : -viewport_width;
            b0 = hp ? 1 : -1;
            d0 = 0;

            //direction from left to right
            a1 = wp ? a0 + 1 : a0 - 1;
            b1 = b0;
            d1 = a1 - a0;

            //apply line length
            w = h;
        }

        //count z coordinate step
        z /= (double)w;

        //Bresenham's algorithm
        mem = x1 + y1 * viewport_width;
        if ((dm != Normal))
            fillCache[y1] = {x1, z1};
        for (w--; w >= 0; w--) {
            if (p < 0) {
                mem += a0;
                y1 += b0;
                p += c0;
                x1 += d0;
            } else {
                mem += a1;
                y1 += b1;
                p += c1;
                x1 += d1;
            }

            //interpolate z coordinate
            z1 += z;

            //write pixel into memory and depth test
            if ((dm != Normal))
                fillCache[y1] = {x1, z1};
            else if ((0 < z1.z) && (depthBuffer[mem] > z1.z)) {
                int v = glm::clamp(z1.z, 0.0, 255.0);
                setColor(v, v, v);
                pixelBuffer[mem] = currentColor;
                depthBuffer[mem] = z1.z;
            }
        }
        return true;
    }
    return false;
}

/**
 * @brief setColor sets RGB current color
 * @param r is red color
 * @param g is green color
 * @param b is blue color
 */
void simple::setColor(unsigned char r, unsigned char g, unsigned char b) {
    currentColor.r = r;
    currentColor.g = g;
    currentColor.b = b;
}

/**
 * @brief test is Liang & Barsky clipping
 */
bool simple::test(double p, double q, double &t1, double &t2) {
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

void simple::triangle(int x1, int y1, int x2, int y2, int x3, int y3, glm::dvec3 z1, glm::dvec3 z2, glm::dvec3 z3) {
    //create markers for filling
    int a12 = glm::abs(y1 - y2);
    int a13 = glm::abs(y1 - y3);
    int a23 = glm::abs(y2 - y3);
    int min, max;
    if ((a12 >= a13) && (a12 >= a23)) {
        line(x1, y1, x2, y2, z1, z2, Marker1);
        line(x1, y1, x3, y3, z1, z3, Marker2);
        line(x2, y2, x3, y3, z2, z3, Marker2);
        min = glm::min(y1, y2);
        max = glm::max(y1, y2);
    } else if ((a13 >= a12) && (a13 >= a23)) {
        line(x1, y1, x3, y3, z1, z3, Marker1);
        line(x1, y1, x2, y2, z1, z2, Marker2);
        line(x2, y2, x3, y3, z2, z3, Marker2);
        min = glm::min(y1, y3);
        max = glm::max(y1, y3);
    } else if ((a23 >= a12) && (a23 >= a13)) {
        line(x2, y2, x3, y3, z2, z3, Marker1);
        line(x1, y1, x2, y2, z1, z2, Marker2);
        line(x1, y1, x3, y3, z1, z3, Marker2);
        min = glm::min(y2, y3);
        max = glm::max(y2, y3);
    }

    //fill triangle
    //setColor(0, 0, 128);
    for (int y = glm::max(0, min); y < glm::min(viewport_height - 1, max); y++) {
        line(fillCache1[y].first, y, fillCache2[y].first, y, fillCache1[y].second, fillCache2[y].second, Normal);
    }

    //draw frames
    /*setColor(255, 0, 0);
    line(x1, y1, x2, y2, z1, z2, Normal);
    line(x1, y1, x3, y3, z1, z3, Normal);
    line(x2, y2, x3, y3, z2, z3, Normal);*/
}

void simple::triangles(float* vertices, int offset, int size) {
    for (int i = offset; i < size; i++) {
        glm::vec4 a = matrix * glm::vec4(vertices[i * 9 + 0], vertices[i * 9 + 1], vertices[i * 9 + 2], 1.0f);
        glm::vec4 b = matrix * glm::vec4(vertices[i * 9 + 3], vertices[i * 9 + 4], vertices[i * 9 + 5], 1.0f);
        glm::vec4 c = matrix * glm::vec4(vertices[i * 9 + 6], vertices[i * 9 + 7], vertices[i * 9 + 8], 1.0f);
        a.x = (a.x / fabs(a.w) + 1.0f) * 0.5f * viewport_width;
        a.y = (a.y / fabs(a.w) + 1.0f) * 0.5f * viewport_height;
        b.x = (b.x / fabs(b.w) + 1.0f) * 0.5f * viewport_width;
        b.y = (b.y / fabs(b.w) + 1.0f) * 0.5f * viewport_height;
        c.x = (c.x / fabs(c.w) + 1.0f) * 0.5f * viewport_width;
        c.y = (c.y / fabs(c.w) + 1.0f) * 0.5f * viewport_height;
        triangle(a.x, a.y, b.x, b.y, c.x, c.y, glm::dvec3(0.0, 0.0, a.z), glm::dvec3(0.0, 0.0, b.z), glm::dvec3(0.0, 0.0, c.z));
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
void simple::lookAt(glm::vec3 eye, glm::vec3 center, glm::vec3 up) {

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
void simple::perspective(float fovy, float aspect, float zNear, float zFar) {
    while(!matrixBuffer.empty()) {
        matrixBuffer.pop();
    }
    proj_matrix = glm::perspective((float)(fovy * M_PI / 180.0f), aspect, zNear,zFar);
    viewDistance = zFar;
}

/**
 * @brief multMatrix multiplies with matrix
 * @param matrix is 4x4 matrix in OpenGL format
 */
void simple::multMatrix(float* matrix) {
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
void simple::popMatrix() {
  /// popping matrix from stack
  matrix_result = (glm::mat4x4)matrixBuffer.top();
  matrixBuffer.pop();
}

/**
 * @brief pushMatrix pushs current matrix to stack
 */
void simple::pushMatrix() {
  /// push matrix m to stack
  matrixBuffer.push(matrix_result);
}

/**
 * @brief rotateX rotate around X axis
 * @param value is angle
 */
void simple::rotateX(float value) {
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
void simple::rotateY(float value) {
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
void simple::rotateZ(float value) {
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
void simple::scale(float value) {
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
void simple::translate(float x, float y, float z) {
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

