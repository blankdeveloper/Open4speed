//----------------------------------------------------------------------------------------
/**
 * \file       modelo4s.cpp
 * \author     Vonasek Lubos
 * \date       2014/02/23
 * \brief      Class for loading models into OpenGL list, it parse full model file, apply
 *             textures, materials and blending.
*/
//----------------------------------------------------------------------------------------

#include "loaders/modelo4s.h"
#include "utils/io.h"
#include "utils/math.h"
#include "utils/switch.h"
#include "common.h"

/**
 * @brief Constructor for loading model from file
 * @param filename is path and name of file to load
 * @param lightmaps is true to load lightmaps
 */
modelo4s::modelo4s(const char* filename, bool lightmaps) {

    models = *(new std::vector<model3d>());

    /// open file
#ifdef ZIP_ARCHIVE
    zip_file* file = zip_fopen(APKArchive, prefix(filename), 0);
#else
    FILE* file = fopen(prefix(filename), "r");
#endif
    char* line = new char[1024];

    /// get model dimensions
    gets(line, file);
    cutX = scandec(line);
    gets(line, file);
    cutY = scandec(line);
    gets(line, file);
    sscanf(line, "%f %f %f %f %f %f", &aabb.min.x, &aabb.min.y, &aabb.min.z, &aabb.max.x, &aabb.max.y, &aabb.max.z);
    width = aabb.max.x - aabb.min.x;
    aplitude = aabb.max.y - aabb.min.y;
    height = aabb.max.z - aabb.min.z;

    /// get amount of textures in model
    gets(line, file);
    int textureCount = scandec(line);

    /// parse all textures
    for (int i = 0; i < textureCount; i++) {

        /// set default value
        model3d *m = new model3d();
        m->reg = new AABB();
        m->triangleCount = new int[cutX * cutY + 1];
        m->usingLightmaps = lightmaps;
        float *colora = new float[4];
        float *colord = new float[4];
        float *colors = new float[4];
        colora[3] = 1;
        colord[3] = 1;
        colors[3] = 1;

        /// get material attributes
        float alpha = 1;
        char texturePath[255];
        char material[255];
        m->lmIndex = -1;
        material[0] = '\0';
        gets(line, file);
        if (lightmaps) {
            sscanf(line, "%f %f %f %f %f %f %s %f %f %f %f %f %f %f %f %f %f %s %d",
                   &m->reg->min.x, &m->reg->min.y, &m->reg->min.z, &m->reg->max.x, &m->reg->max.y, &m->reg->max.z,
                   &texturePath[0], &colora[0], &colora[1], &colora[2], &colord[0], &colord[1], &colord[2],
                   &colors[0], &colors[1], &colors[2], &alpha, &material[0], &m->lmIndex);
        } else {
            sscanf(line, "%f %f %f %f %f %f %s %f %f %f %f %f %f %f %f %f %f %s",
                   &m->reg->min.x, &m->reg->min.y, &m->reg->min.z, &m->reg->max.x, &m->reg->max.y, &m->reg->max.z,
                   &texturePath[0], &colora[0], &colora[1], &colora[2], &colord[0], &colord[1], &colord[2],
                   &colors[0], &colors[1], &colors[2], &alpha, &material[0]);
        }

        /// get model size
        m->x = m->reg->min.x;
        m->y = m->reg->min.y;
        m->z = m->reg->min.z;

        /// if texture is not only single color then load it
        if (texturePath[0] != '*') {
            m->texture2D = getTexture(texturePath, alpha);
        /// create color texture
        } else {
            m->texture2D = xrenderer->getGrayTexture();
        }

        int cursor = 0;
        m->dynamic = false;
        m->filter = 0;
        m->touchable = true;
        if (m->texture2D->transparent) {
            m->material = getShader("standart_alpha");
        } else {
            m->material = getShader("standart");
        }

        /// get material parameters
        while(true) {
            if (material[cursor] == '!') {
                m->touchable = false;
                cursor++;
            } else if (material[cursor] == '$') {
                m->dynamic = true;
                cursor++;
            } else if (material[cursor] == '#') {
                cursor++;
                m->filter = material[cursor] - '0';
                cursor++;
            } else if (material[cursor] == '%') {
                cursor++;
                char* shadername = new char[strlen(material) - cursor + 1];
                for (unsigned int j = cursor; j < strlen(material); j++) {
                    shadername[j - cursor] = material[j];
                    if (material[j] == '/') {
                        shadername[j - cursor] = '\000';
                        break;
                    }
                }
                shadername[strlen(material) - cursor] = '\000';
                m->material = getShader(shadername);
                break;
            } else {
                break;
            }
        }

        /// prepare model arrays
        m->triangleCount[0] = 0;
        for (int j = 1; j <= cutX * cutY; j++) {
            gets(line, file);
            m->triangleCount[j] = scandec(line);
        }
        m->vertices = new float[m->triangleCount[cutX * cutY] * 3 * 3];
        m->normals = new float[m->triangleCount[cutX * cutY] * 3 * 3];
        m->coords = new float[m->triangleCount[cutX * cutY] * 3 * 2];
        m->tid = new float[m->triangleCount[cutX * cutY] * 3 * 2];
        for (int j = 0; j < m->triangleCount[cutX * cutY]; j++) {
            for (int k = 0; k < 6; k++) {
                m->tid[j * 6 + 0 + k] = 4095 / 4096.0;
            }
        }
        m->colora = colora;
        m->colord = colord;
        m->colors = colors;
        for (int j = 0; j < m->triangleCount[cutX * cutY]; j++) {
            /// read triangle parameters
            gets(line, file);
            if (lightmaps) {
                int x, y, u, v;
                sscanf(line, "%f %f %f %f %f %f %f %f%f %f %f %f %f %f %f %f%f %f %f %f %f %f %f %f %d %d %d %d",
                       &m->coords[j * 3 * 2 + 0], &m->coords[j * 3 * 2 + 1],
                       &m->normals[j * 3 * 3 + 0], &m->normals[j * 3 * 3 + 1], &m->normals[j * 3 * 3 + 2],
                       &m->vertices[j * 3 * 3 + 0], &m->vertices[j * 3 * 3 + 1], &m->vertices[j * 3 * 3 + 2],
                       &m->coords[j * 3 * 2 + 2], &m->coords[j * 3 * 2 + 3],
                       &m->normals[j * 3 * 3 + 3], &m->normals[j * 3 * 3 + 4], &m->normals[j * 3 * 3 + 5],
                       &m->vertices[j * 3 * 3 + 3], &m->vertices[j * 3 * 3 + 4], &m->vertices[j * 3 * 3 + 5],
                       &m->coords[j * 3 * 2 + 4], &m->coords[j * 3 * 2 + 5],
                       &m->normals[j * 3 * 3 + 6], &m->normals[j * 3 * 3 + 7], &m->normals[j * 3 * 3 + 8],
                       &m->vertices[j * 3 * 3 + 6], &m->vertices[j * 3 * 3 + 7], &m->vertices[j * 3 * 3 + 8],
                       &x, &y, &u, &v);

                float d02 = dist(m->vertices[j * 3 * 3 + 0], m->vertices[j * 3 * 3 + 1], m->vertices[j * 3 * 3 + 2],
                                 m->vertices[j * 3 * 3 + 3], m->vertices[j * 3 * 3 + 4], m->vertices[j * 3 * 3 + 5]);
                float d04 = dist(m->vertices[j * 3 * 3 + 0], m->vertices[j * 3 * 3 + 1], m->vertices[j * 3 * 3 + 2],
                                 m->vertices[j * 3 * 3 + 6], m->vertices[j * 3 * 3 + 7], m->vertices[j * 3 * 3 + 8]);
                float d24 = dist(m->vertices[j * 3 * 3 + 3], m->vertices[j * 3 * 3 + 4], m->vertices[j * 3 * 3 + 5],
                                 m->vertices[j * 3 * 3 + 6], m->vertices[j * 3 * 3 + 7], m->vertices[j * 3 * 3 + 8]);
                int a = 0;
                int b = 2;
                int c = 4;
                if ((d02 >= d04) && (d04 >= d24)) {
                    a = 4;
                    b = 2;
                    c = 0;
                }
                if ((d02 >= d24) && (d24 >= d04)) {
                    a = 4;
                    b = 0;
                    c = 2;
                }
                if ((d04 >= d02) && (d02 >= d24)) {
                    a = 2;
                    b = 4;
                    c = 0;
                }
                if ((d04 >= d24) && (d24 >= d02)) {
                    a = 2;
                    b = 0;
                    c = 4;
                }
                if ((d24 >= d02) && (d02 >= d04)) {
                    a = 0;
                    b = 4;
                    c = 2;
                }
                if ((d24 >= d04) && (d04 >= d02)) {
                    a = 0;
                    b = 2;
                    c = 4;
                }
                if (u > 0) {
                    m->tid[j * 6 + c] = (x + u) / 256.0 - 2 / (float)rttsize;
                    m->tid[j * 6 + c + 1] = y / 256.0 + 2 / (float)rttsize;
                    m->tid[j * 6 + b] = x / 256.0 + 2 / (float)rttsize;
                    m->tid[j * 6 + b + 1] = (y + v) / 256.0 - 2 / (float)rttsize;
                    m->tid[j * 6 + a] = x / 256.0 + 2 / (float)rttsize;
                    m->tid[j * 6 + a + 1] = y / 256.0 + 2 / (float)rttsize;
                } else {
                    m->tid[j * 6 + c] = x / 256.0;
                    m->tid[j * 6 + c + 1] = (y + v) / 256.0 + 4 / (float)rttsize;
                    m->tid[j * 6 + a] = x / 256.0;
                    m->tid[j * 6 + a + 1] = y / 256.0;
                    m->tid[j * 6 + b] = (x + u) / 256.0 + 4 / (float)rttsize;
                    m->tid[j * 6 + b + 1] = y / 256.0;
                }
                if (renderLightmap) {
                    m->coords[j * 6 + 0] = 0;
                    m->coords[j * 6 + 1] = 0;

                    m->coords[j * 6 + 2] = 1;
                    m->coords[j * 6 + 3] = 0;

                    m->coords[j * 6 + 4] = 0;
                    m->coords[j * 6 + 5] = 1;
                }

            } else {
                sscanf(line, "%f %f %f %f %f %f %f %f%f %f %f %f %f %f %f %f%f %f %f %f %f %f %f %f",
                       &m->coords[j * 3 * 2 + 0], &m->coords[j * 3 * 2 + 1],
                       &m->normals[j * 3 * 3 + 0], &m->normals[j * 3 * 3 + 1], &m->normals[j * 3 * 3 + 2],
                       &m->vertices[j * 3 * 3 + 0], &m->vertices[j * 3 * 3 + 1], &m->vertices[j * 3 * 3 + 2],
                       &m->coords[j * 3 * 2 + 2], &m->coords[j * 3 * 2 + 3],
                       &m->normals[j * 3 * 3 + 3], &m->normals[j * 3 * 3 + 4], &m->normals[j * 3 * 3 + 5],
                       &m->vertices[j * 3 * 3 + 3], &m->vertices[j * 3 * 3 + 4], &m->vertices[j * 3 * 3 + 5],
                       &m->coords[j * 3 * 2 + 4], &m->coords[j * 3 * 2 + 5],
                       &m->normals[j * 3 * 3 + 6], &m->normals[j * 3 * 3 + 7], &m->normals[j * 3 * 3 + 8],
                       &m->vertices[j * 3 * 3 + 6], &m->vertices[j * 3 * 3 + 7], &m->vertices[j * 3 * 3 + 8]);
            }
        }

        /// store model in VBO
        int size = sizeof(float)*m->triangleCount[cutX * cutY] * 3;
        m->vboData = getVBO(size, m->vertices, m->normals, m->coords, m->tid);
        models.push_back(*m);
    }

    /// load edges
    gets(line, file);
    edgesCount = scandec(line);
    edges = new std::vector<edge>[edgesCount];
    for (int i = 0; i < edgesCount; i++) {
        gets(line, file);
        int edgeCount = scandec(line);
        for (int j = 0; j < edgeCount; j++) {
            edge value = *(new edge());
            gets(line, file);
            sscanf(line, "%f %f %f %f %f %f", &value.ax, &value.ay, &value.az, &value.bx, &value.by, &value.bz);
            edges[i].push_back(value);
        }
        for (int j = 0; j < edgeCount; j++) {
            edge value = *(new edge());
            value.ax = edges[i][j].bx;
            value.ay = edges[i][j].by;
            value.az = edges[i][j].bz;
            value.bx = edges[i][j].ax;
            value.by = edges[i][j].ay;
            value.bz = edges[i][j].az;
            edges[i].push_back(value);
        }
    }

    delete[] line;
#ifdef ZIP_ARCHIVE
    zip_fclose(file);
#else
    fclose(file);
#endif
}

/**
 * @brief getLMCount get amount of lightmaps
 * @return lightmaps count
 */
int modelo4s::getLMCount() {
    int max = 0;
    for (unsigned int i = 0; i < models.size(); i++) {
        if (max < models[i].lmIndex) {
            max = models[i].lmIndex;
        }
    }
    return max + 1;
}
