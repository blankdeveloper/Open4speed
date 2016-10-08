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


//http://stackoverflow.com/questions/23880160/stdmap-key-no-match-for-operator
bool operator<(const id3d& lhs, const id3d& rhs)
{
    return lhs.x < rhs.x ||
           lhs.x == rhs.x && (lhs.y < rhs.y || lhs.y == rhs.y && lhs.z < rhs.z);
}

/**
 * @brief model destructor
 */
model::~model() {
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
        int count = f->scandec();
        float v[9];
        float n[9];
        float t[6];
        for (int j = 0; j < count; j++) {
            /// read triangle parameters
            f->gets(line);
            sscanf(line, "%f %f %f %f %f %f %f %f%f %f %f %f %f %f %f %f%f %f %f %f %f %f %f %f",
                   &t[0], &t[1], &n[0], &n[1], &n[2], &v[0], &v[1], &v[2],
                   &t[2], &t[3], &n[3], &n[4], &n[5], &v[3], &v[4], &v[5],
                   &t[4], &t[5], &n[6], &n[7], &n[8], &v[6], &v[7], &v[8]);

            /// copy data
            for(int i = 0; i < 9; i++)
                m.vertices.push_back(v[i]);
            for(int i = 0; i < 9; i++)
                m.normals.push_back(n[i]);
            for(int i = 0; i < 6; i++)
                m.coords.push_back(t[i]);
        }
        models.push_back(m);
    }
    delete f;
}

void model::detexturise(bool culling) {
    for (unsigned int i = 0; i < models.size(); i++)
        if((models[i].filter == 0) && !models[i].dynamic && !models[i].touchable)
        {
            model3d* m = &models[i];
            long v = 0;
            long t = 0;
            id3d id;
            id.x = 0;
            id.y = 0;
            id.z = 0;
            glm::vec3 a, b, c, center;
            ColorRGBA ca, cb, cc;
            for (unsigned int i = 0; i < m->vertices.size() / 9; i++, v += 9, t += 6) {
                //get vertices
                a = glm::vec3(m->vertices[v + 0] + m->reg.min.x,
                              m->vertices[v + 1] + m->reg.min.y,
                              m->vertices[v + 2] + m->reg.min.z);
                b = glm::vec3(m->vertices[v + 3] + m->reg.min.x,
                              m->vertices[v + 4] + m->reg.min.y,
                              m->vertices[v + 5] + m->reg.min.z);
                c = glm::vec3(m->vertices[v + 6] + m->reg.min.x,
                              m->vertices[v + 7] + m->reg.min.y,
                              m->vertices[v + 8] + m->reg.min.z);
                //get colors
                ca = m->texture2D->getPixel(m->coords[t + 0], m->coords[t + 1]);
                cb = m->texture2D->getPixel(m->coords[t + 2], m->coords[t + 3]);
                cc = m->texture2D->getPixel(m->coords[t + 4], m->coords[t + 5]);
                //get id
                center = (a + b + c) / 3.0f;
                if(culling)
                {
                    id.x = center.x / 150;
                    id.y = center.y / 150;
                    id.z = center.z / 150;
                }
                //put triangle into data
                vertices[id].push_back(a.x);
                vertices[id].push_back(a.y);
                vertices[id].push_back(a.z);
                vertices[id].push_back(b.x);
                vertices[id].push_back(b.y);
                vertices[id].push_back(b.z);
                vertices[id].push_back(c.x);
                vertices[id].push_back(c.y);
                vertices[id].push_back(c.z);
                colors[id].push_back(ca.r / 255.0f);
                colors[id].push_back(ca.g / 255.0f);
                colors[id].push_back(ca.b / 255.0f);
                colors[id].push_back(cb.r / 255.0f);
                colors[id].push_back(cb.g / 255.0f);
                colors[id].push_back(cb.b / 255.0f);
                colors[id].push_back(cc.r / 255.0f);
                colors[id].push_back(cc.g / 255.0f);
                colors[id].push_back(cc.b / 255.0f);
            }
        }
}
