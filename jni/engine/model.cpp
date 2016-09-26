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
        if (models[i].vboData)
             delete models[i].vboData;
        delete[] models[i].vertices;
        if (models[i].normals)
             delete[] models[i].normals;
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
        m.triangleCount = f->scandec();
        m.vboData = 0;
        m.vertices = new float[m.triangleCount * 9];
        m.normals = new float[m.triangleCount * 9];
        m.coords = new float[m.triangleCount * 6];
        for (int j = 0; j < m.triangleCount; j++) {
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

#ifdef USE_VBO
        /// store model in VBO
        if (!m.touchable) {
            int size = m.triangleCount * 3;
            if (!m.material->hasAttrib(1)) {
                delete[] m.normals;
                m.normals = 0;
            }
            if (!m.material->hasAttrib(2)) {
                delete[] m.coords;
                m.coords = 0;
            }
            m.vboData = getVBO(size, m.vertices, m.normals, m.coords);
        }
#endif
        models.push_back(m);
    }

    delete f;
}
