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
#include "utils/switch.h"

/**
 * @brief Constructor for loading model from file
 * @param filename is path and name of file to load
 */
modelo4s::modelo4s(std::string filename) {

    /// open file
#ifdef ZIP_ARCHIVE
    zip_file* file = zip_fopen(APKArchive, prefix(filename).c_str(), 0);
#else
    FILE* file = fopen(prefix(filename).c_str(), "r");
#endif

    /// get model dimensions
    char line[1024];
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
        model3d m;
        m.triangleCount = new int[cutX * cutY + 1];
        m.colora[3] = 1;
        m.colord[3] = 1;
        m.colors[3] = 1;

        /// get material attributes
        float alpha = 1;
        char texturePath[255];
        char material[255];
        material[0] = '\0';
        gets(line, file);
        sscanf(line, "%f %f %f %f %f %f %s %f %f %f %f %f %f %f %f %f %f %s",
               &m.reg.min.x, &m.reg.min.y, &m.reg.min.z, &m.reg.max.x, &m.reg.max.y, &m.reg.max.z,
               &texturePath[0], &m.colora[0], &m.colora[1], &m.colora[2], &m.colord[0], &m.colord[1], &m.colord[2],
               &m.colors[0], &m.colors[1], &m.colors[2], &alpha, &material[0]);

        /// get model size
        m.x = m.reg.min.x;
        m.y = m.reg.min.y;
        m.z = m.reg.min.z;

        /// if texture is not only single color then load it
        if (texturePath[0] != '*') {
            m.texture2D = getTexture(texturePath, alpha);
        /// create color texture
        } else {
            m.texture2D = getTexture(m.colord[0], m.colord[1], m.colord[2], alpha);
        }

        int cursor = 0;
        m.dynamic = false;
        m.filter = 0;
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
            } else if (material[cursor] == '$') {
                m.dynamic = true;
                cursor++;
            } else if (material[cursor] == '#') {
                cursor++;
                m.filter = material[cursor] - '0';
                cursor++;
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
            } else {
                break;
            }
        }

        /// prepare model arrays
        m.triangleCount[0] = 0;
        for (int j = 1; j <= cutX * cutY; j++) {
            gets(line, file);
            m.triangleCount[j] = scandec(line);
        }
        m.vertices = new float[m.triangleCount[cutX * cutY] * 9];
        m.normals = new float[m.triangleCount[cutX * cutY] * 9];
        m.coords = new float[m.triangleCount[cutX * cutY] * 6];
        m.tnormals = new float[m.triangleCount[cutX * cutY] * 9];
        for (int j = 0; j < m.triangleCount[cutX * cutY]; j++) {
            /// read triangle parameters
            gets(line, file);
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

            m.tnormals[j * 9 + 0] = glm::normalize(m.normals[j * 9 + 0] + m.normals[j * 9 + 3] + m.normals[j * 9 + 6]);
            m.tnormals[j * 9 + 1] = glm::normalize(m.normals[j * 9 + 1] + m.normals[j * 9 + 4] + m.normals[j * 9 + 7]);
            m.tnormals[j * 9 + 2] = glm::normalize(m.normals[j * 9 + 2] + m.normals[j * 9 + 5] + m.normals[j * 9 + 8]);
            m.tnormals[j * 9 + 3] = m.tnormals[j * 9 + 0];
            m.tnormals[j * 9 + 4] = m.tnormals[j * 9 + 1];
            m.tnormals[j * 9 + 5] = m.tnormals[j * 9 + 2];
            m.tnormals[j * 9 + 6] = m.tnormals[j * 9 + 0];
            m.tnormals[j * 9 + 7] = m.tnormals[j * 9 + 1];
            m.tnormals[j * 9 + 8] = m.tnormals[j * 9 + 2];
        }

        /// store model in VBO
        if (!m.touchable) {
            int size = m.triangleCount[cutX * cutY] * 3;
            if (!m.material->hasAttrib(1)) {
                delete[] m.normals;
                m.normals = 0;
            }
            if (!m.material->hasAttrib(2)) {
                delete[] m.coords;
                m.coords = 0;
            }
            if (!m.material->hasAttrib(3)) {
                delete[] m.tnormals;
                m.tnormals = 0;
            }
            m.vboData = getVBO(size, m.vertices, m.normals, m.coords, m.tnormals, false);
        } else
            m.vboData = 0;
        models.push_back(m);
    }

#ifdef ZIP_ARCHIVE
    zip_fclose(file);
#else
    fclose(file);
#endif
}
