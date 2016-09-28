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

#define VOXEL_DENSITY 1

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

//https://gist.github.com/yamamushi/5823518
void model::bresenham3D(model3d* model, long x1, long y1, long z1, const long x2, const long y2, const long z2, std::vector<long> *output) {

    long i, dx, dy, dz, l, m, n, x_inc, y_inc, z_inc, err_1, err_2, dx2, dy2, dz2;
    long point[3];

    point[0] = x1;
    point[1] = y1;
    point[2] = z1;
    dx = x2 - x1;
    dy = y2 - y1;
    dz = z2 - z1;
    x_inc = (dx < 0) ? -1 : 1;
    l = abs(dx);
    y_inc = (dy < 0) ? -1 : 1;
    m = abs(dy);
    z_inc = (dz < 0) ? -1 : 1;
    n = abs(dz);
    dx2 = l << 1;
    dy2 = m << 1;
    dz2 = n << 1;

    if ((l >= m) && (l >= n)) {
        err_1 = dy2 - l;
        err_2 = dz2 - l;
        for (i = 0; i <= l; i++) {
            if(output) {
                output->push_back(point[0]);
                output->push_back(point[1]);
                output->push_back(point[2]);
            } else {
                model->voxelCoord.push_back(point[0] / (float)VOXEL_DENSITY);
                model->voxelCoord.push_back(point[1] / (float)VOXEL_DENSITY);
                model->voxelCoord.push_back(point[2] / (float)VOXEL_DENSITY);
                model->voxelColor.push_back(128);
                model->voxelColor.push_back(128);
                model->voxelColor.push_back(128);
            }
            if (err_1 > 0) {
                point[1] += y_inc;
                err_1 -= dx2;
            }
            if (err_2 > 0) {
                point[2] += z_inc;
                err_2 -= dx2;
            }
            err_1 += dy2;
            err_2 += dz2;
            point[0] += x_inc;
        }
    } else if ((m >= l) && (m >= n)) {
        err_1 = dx2 - m;
        err_2 = dz2 - m;
        for (i = 0; i <= m; i++) {
            if(output) {
                output->push_back(point[0]);
                output->push_back(point[1]);
                output->push_back(point[2]);
            } else {
                model->voxelCoord.push_back(point[0] / (float)VOXEL_DENSITY);
                model->voxelCoord.push_back(point[1] / (float)VOXEL_DENSITY);
                model->voxelCoord.push_back(point[2] / (float)VOXEL_DENSITY);
                model->voxelColor.push_back(128);
                model->voxelColor.push_back(128);
                model->voxelColor.push_back(128);
            }
            if (err_1 > 0) {
                point[0] += x_inc;
                err_1 -= dy2;
            }
            if (err_2 > 0) {
                point[2] += z_inc;
                err_2 -= dy2;
            }
            err_1 += dx2;
            err_2 += dz2;
            point[1] += y_inc;
        }
    } else {
        err_1 = dy2 - n;
        err_2 = dx2 - n;
        for (i = 0; i <= n; i++) {
            if(output) {
                output->push_back(point[0]);
                output->push_back(point[1]);
                output->push_back(point[2]);
            } else {
                model->voxelCoord.push_back(point[0] / (float)VOXEL_DENSITY);
                model->voxelCoord.push_back(point[1] / (float)VOXEL_DENSITY);
                model->voxelCoord.push_back(point[2] / (float)VOXEL_DENSITY);
                model->voxelColor.push_back(128);
                model->voxelColor.push_back(128);
                model->voxelColor.push_back(128);
            }
            if (err_1 > 0) {
                point[1] += y_inc;
                err_1 -= dz2;
            }
            if (err_2 > 0) {
                point[0] += x_inc;
                err_2 -= dz2;
            }
            err_1 += dy2;
            err_2 += dx2;
            point[2] += z_inc;
        }
    }
}

void model::triangles(model3d* m) {
    long v = 0;
    long t = 0;
    long ab, ac, bc;
    glm::vec3 a, b, c;
    glm::dvec3 az, bz, cz;
    for (int i = 0; i < m->count; i++, v += 9, t += 6) {
        //get vertices
        a = glm::vec3(m->vertices[v + 0], m->vertices[v + 1], m->vertices[v + 2]);
        b = glm::vec3(m->vertices[v + 3], m->vertices[v + 4], m->vertices[v + 5]);
        c = glm::vec3(m->vertices[v + 6], m->vertices[v + 7], m->vertices[v + 8]);
        //set density
        a *= VOXEL_DENSITY;
        b *= VOXEL_DENSITY;
        c *= VOXEL_DENSITY;
        a += 0.5f;
        b += 0.5f;
        c += 0.5f;
        //get texture coordinates and depth for interpolation
        az = glm::dvec3(m->coords[t + 0], m->coords[t + 1], a.z);
        bz = glm::dvec3(m->coords[t + 2], m->coords[t + 3], b.z);
        cz = glm::dvec3(m->coords[t + 4], m->coords[t + 5], c.z);

        //find edges
        ab = glm::length(a - b);
        ac = glm::length(a - c);
        bc = glm::length(b - c);
        std::vector<long> output1;
        std::vector<long> output2;
        if ((ab >= ac) && (ab >= bc)) {
            bresenham3D(m, a.x, a.y, a.z, c.x, c.y, c.z, &output1);
            bresenham3D(m, b.x, b.y, b.z, c.x, c.y, c.z, &output2);
        } else if ((ac >= ab) && (ac >= bc)) {
            bresenham3D(m, a.x, a.y, a.z, b.x, b.y, b.z, &output1);
            bresenham3D(m, c.x, c.y, c.z, b.x, b.y, b.z, &output2);
        } else if ((bc >= ab) && (bc >= ac)) {
            bresenham3D(m, a.x, a.y, a.z, b.x, b.y, b.z, &output1);
            bresenham3D(m, a.x, a.y, a.z, c.x, c.y, c.z, &output2);
        }

        //fill
        unsigned int min = glm::min(output1.size(), output2.size()) / 3 - 1;
        unsigned int max = glm::max(output1.size(), output2.size()) / 3 - 1;
        for (unsigned int j = 0; j <= min; j++) {
            bresenham3D(m, output1[j * 3 + 0], output1[j * 3 + 1], output1[j * 3 + 2], output2[j * 3 + 0], output2[j * 3 + 1], output2[j * 3 + 2], 0);
        }
        for (unsigned int j = min + 1; j <= max; j++) {
            if (output1.size() > output2.size())
                bresenham3D(m, output1[j * 3 + 0], output1[j * 3 + 1], output1[j * 3 + 2], output2[min * 3 + 0], output2[min * 3 + 1], output2[min * 3 + 2], 0);
            else
                bresenham3D(m, output1[min * 3 + 0], output1[min * 3 + 1], output1[min * 3 + 2], output2[j * 3 + 0], output2[j * 3 + 1], output2[j * 3 + 2], 0);
        }
    }
    m->material = getShader("voxel");
    m->voxelCount = m->voxelCoord.size() / 3;
    printf("Converted %d voxels\n", m->voxelCount);
}

void model::voxelise() {
    for (unsigned int i = 0; i < models.size(); i++)
        triangles(&models[i]);
    voxelised = true;
}
