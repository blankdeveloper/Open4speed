///----------------------------------------------------------------------------------------
/**
 * \file       model.h
 * \author     Vonasek Lubos
 * \date       2014/12/31
 * \brief      Loading and storing models
**/
///----------------------------------------------------------------------------------------

#include "engine/io.h"
#include "engine/model.h"
#include "engine/switch.h"

/**
 * @brief model destructor
 */
model::~model() {
    for (unsigned int i = 0; i < models.size(); i++) {
        delete[] models[i].vertices;
        if (models[i].normals)
            delete[] models[i].normals;
        if (models[i].coords)
            delete[] models[i].coords;
    }
}

/**
 * @brief Constructor for loading model from file
 * @param filename is path and name of file to load
 */
model::model(std::string filename) {

    /// open file
    file* f = getFile(filename);

    /// get model dimensions
    char line[1024];
    f->gets(line);
    sscanf(line, "%f %f %f %f %f %f", &aabb.min.x, &aabb.min.y, &aabb.min.z, &aabb.max.x, &aabb.max.y, &aabb.max.z);
    width = aabb.max.x - aabb.min.x;
    aplitude = aabb.max.y - aabb.min.y;
    height = aabb.max.z - aabb.min.z;

    /// get amount of textures in model
    int textureCount = f->scandec();

    /// parse all textures
    for (int i = 0; i < textureCount; i++) {

        /// set default value
        model3d m;
        m.colora[3] = 1;
        m.colord[3] = 1;
        m.colors[3] = 1;

        /// get material attributes
        float alpha = 1; //unused, remove from format
        char texturePath[255];
        char material[255];
        material[0] = '\0';
        f->gets(line);
        sscanf(line, "%f %f %f %f %f %f %s %f %f %f %f %f %f %f %f %f %f %s",
               &m.reg.min.x, &m.reg.min.y, &m.reg.min.z, &m.reg.max.x, &m.reg.max.y, &m.reg.max.z,
               &texturePath[0], &m.colora[0], &m.colora[1], &m.colora[2], &m.colord[0], &m.colord[1], &m.colord[2],
               &m.colors[0], &m.colors[1], &m.colors[2], &alpha, &material[0]);

        /// get model size
        m.x = m.reg.min.x;
        m.y = m.reg.min.y;
        m.z = m.reg.min.z;

        /// if texture is not only single color then load it
        if((texturePath[0] != '*') && (texturePath[0] != '('))
            m.texture2D = getTexture(f->path() + texturePath);
        /// create color texture
        else
            m.texture2D = getTexture(m.colord[0], m.colord[1], m.colord[2]);

        int cursor = 0;
        m.dynamic = false;
        m.filter = 0;
        m.hasShadow = false;
        m.touchable = false;
        if (m.texture2D->transparent) {
            m.material = getShader("standart_alpha");
        } else {
            m.material = getShader("standart");
        }

        /// get material parameters
        while(true) {
            if (material[cursor] == '!') {
                m.touchable = true;
                cursor++;
            } else if (material[cursor] == '@') {
                m.hasShadow = true;
                cursor++;
            } else if (material[cursor] == '$') {
                m.dynamic = true;
                cursor++;
            } else if (material[cursor] == '#') {
                cursor++;
                m.filter = material[cursor] - '0';
                cursor++;
#ifndef SOFTWARE_RENDERER
            } else if (material[cursor] == '%') {
                cursor++;
                m.texture2D->transparent = false;
                char* shadername = new char[strlen(material) - cursor + 1];
                for (unsigned int j = cursor; j < strlen(material); j++) {
                    shadername[j - cursor] = material[j];
                    if (material[j] == '/') {
                        shadername[j - cursor] = '\000';
                        break;
                    }
                }
                shadername[strlen(material) - cursor] = '\000';
                m.material = getShader(shadername);
                delete[] shadername;
                break;
#endif
            } else {
                break;
            }
        }

        /// prepare model arrays
        m.count = f->scandec();
        m.vertices = new float[m.count * 9];
        m.normals = new float[m.count * 9];
        m.coords = new float[m.count * 6];
        for (int j = 0; j < m.count; j++) {
            /// read triangle parameters
            f->gets(line);
            sscanf(line, "%f %f %f %f %f %f %f %f%f %f %f %f %f %f %f %f%f %f %f %f %f %f %f %f",
                   &m.coords[j * 6 + 0], &m.coords[j * 6 + 1],
                   &m.normals[j * 9 + 0], &m.normals[j * 9 + 1], &m.normals[j * 9 + 2],
                   &m.vertices[j * 9 + 0], &m.vertices[j * 9 + 1], &m.vertices[j * 9 + 2],

                   &m.coords[j * 6 + 2], &m.coords[j * 6 + 3],
                   &m.normals[j * 9 + 3], &m.normals[j * 9 + 4], &m.normals[j * 9 + 5],
                   &m.vertices[j * 9 + 3], &m.vertices[j * 9 + 4], &m.vertices[j * 9 + 5],

                   &m.coords[j * 6 + 4], &m.coords[j * 6 + 5],
                   &m.normals[j * 9 + 6], &m.normals[j * 9 + 7], &m.normals[j * 9 + 8],
                   &m.vertices[j * 9 + 6], &m.vertices[j * 9 + 7], &m.vertices[j * 9 + 8]);
        }
        models.push_back(m);
    }
    voxelised = false;
    delete f;
}

/**
  * @brief line makes markers for filling triangle
  * @param x1 is line start position x
  * @param y1 is line start position y
  * @param x2 is line end position x
  * @param y2 is line end position y
  * @param fillCache is cache to be used
  * @return true if line was processed
  */
void model::line(int x1, int y1, int x2, int y2, glm::dvec3 z1, glm::dvec3 z2, std::pair<int, glm::dvec3>* fillCache) {

    int o = aabb.min.z + 1;
    glm::dvec3 z;
    std::pair<int, glm::dvec3> v;
    int c0, c1, xp0, xp1, yp0, yp1, y, p, w, h;
    bool wp, hp;

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
    fillCache[y - o] = v;
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
            fillCache[y - o] = v;
        }
    }
}

void model::triangles(model3d* m) {
    int v = 0;
    int t = 0;
    int o = aabb.min.z + 1;
    int ab, ac, bc, step, x, x1, x2, y, min, mem, max;
    glm::vec4 a, b, c;
    glm::dvec3 az, bz, cz, z, z1, z2;

    std::pair<int, glm::dvec3> fillCache1[(int)height + 1];
    std::pair<int, glm::dvec3> fillCache2[(int)height + 1];
    std::vector<float> vertices;
    std::vector<float> colors;

    for (int i = 0; i < m->count; i++, v += 9, t += 6) {
        //get vertices
        a = glm::vec4(m->vertices[v + 0], m->vertices[v + 1], m->vertices[v + 2], 1.0f);
        b = glm::vec4(m->vertices[v + 3], m->vertices[v + 4], m->vertices[v + 5], 1.0f);
        c = glm::vec4(m->vertices[v + 6], m->vertices[v + 7], m->vertices[v + 8], 1.0f);
        //get texture coordinates and depth for interpolation
        az = glm::dvec3(m->coords[t + 0], m->coords[t + 1], a.z);
        bz = glm::dvec3(m->coords[t + 2], m->coords[t + 3], b.z);
        cz = glm::dvec3(m->coords[t + 4], m->coords[t + 5], c.z);

        //create markers for filling
        ab = glm::abs(a.y - b.y);
        ac = glm::abs(a.y - c.y);
        bc = glm::abs(b.y - c.y);
        if ((ab >= ac) && (ab >= bc)) {
            line(a.x, a.y, b.x, b.y, az, bz, fillCache1);
            line(a.x, a.y, c.x, c.y, az, cz, fillCache2);
            line(b.x, b.y, c.x, c.y, bz, cz, fillCache2);
            min = glm::max(0.0f, glm::min(a.y, b.y));
            max = glm::min(glm::max(a.y, b.y), (int)height - 1.0f);
        } else if ((ac >= ab) && (ac >= bc)) {
            line(a.x, a.y, c.x, c.y, az, cz, fillCache1);
            line(a.x, a.y, b.x, b.y, az, bz, fillCache2);
            line(b.x, b.y, c.x, c.y, bz, cz, fillCache2);
            min = glm::max(0.0f, glm::min(a.y, c.y));
            max = glm::min(glm::max(a.y, c.y), (int)height - 1.0f);
        } else if ((bc >= ab) && (bc >= ac)) {
            line(b.x, b.y, c.x, c.y, bz, cz, fillCache1);
            line(a.x, a.y, b.x, b.y, az, bz, fillCache2);
            line(a.x, a.y, c.x, c.y, az, cz, fillCache2);
            min = glm::max(0.0f, glm::min(b.y, c.y));
            max = glm::min(glm::max(b.y, c.y), (int)height - 1.0f);
        }

        //fill triangle
        for (y = min; y <= max; y++) {
            x1 = fillCache1[y - o].first;
            x2 = fillCache2[y - o].first;
            z1 = fillCache1[y - o].second;
            z2 = fillCache2[y - o].second;

            //filling algorithm initialize
            x = x1;
            step = (x2 >= x1) ? 1 : -1;
            z = (z2 - z1) / (double)glm::abs(x2 - x1);
            mem = glm::abs(x2 - x1);
            for (; mem >= 0; mem--) {

                //write pixel into memory
                //Color c = {128, 128, 128, 255};
                ColorRGBA c = m->texture2D->getPixel(z1.x, z1.y);
                if(c.a > 128) {
                    for(min = (int)z1.z; min < (int)(z1.z + z.z); min++) {
                        vertices.push_back(x);
                        vertices.push_back(y);
                        vertices.push_back(min);
                        colors.push_back(c.r / 255.0f);
                        colors.push_back(c.g / 255.0f);
                        colors.push_back(c.b / 255.0f);
                    }
                }

                //interpolate
                x += step;
                z1 += z;
            }
        }
    }

    //import into model
    delete[] m->vertices;
    if (m->normals)
        delete[] m->normals;
    if(m->coords)
      delete[] m->coords;
    m->coords = 0;
    m->material = getShader("voxel");
    m->vertices = new float[vertices.size()];
    m->normals = new float[vertices.size()];
    m->count = vertices.size() / 3;
    for(unsigned int i = 0; i < vertices.size(); i++) {
      m->vertices[i] = vertices[i];
      m->normals[i] = colors[i];
    }
    printf("Converted %d voxels\n", vertices.size() / 3);
    //m->count *= 3;
    //m->material = getShader("voxel");
}

void model::voxelise() {
    for (unsigned int i = 0; i < models.size(); i++)
        triangles(&models[i]);
    voxelised = true;
}
