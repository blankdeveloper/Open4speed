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
model::~model()
{
}

/**
 * @brief Constructor for loading model from file
 * @param filename is path and name of file to load
 */
model::model(std::string filename)
{
    /// open file
    file* f = getFile(filename);

    /// get model dimensions
    char line[1024];
    f->gets(line);
    sscanf(line, "%f %f %f %f %f %f", &aabb.min.x, &aabb.min.y, &aabb.min.z, &aabb.max.x, &aabb.max.y, &aabb.max.z);

    /// get amount of textures in model
    int textureCount = f->scandec();

    /// parse all textures
    for (int i = 0; i < textureCount; i++)
    {
        //TODO:unused, remove from format
        float alpha = 1;
        float colora[4];
        float colord[4];
        float colors[4];

        /// get material attributes
        model3d m;
        char texturePath[255];
        char material[255];
        material[0] = '\0';
        f->gets(line);
        sscanf(line, "%f %f %f %f %f %f %s %f %f %f %f %f %f %f %f %f %f %s",
               &m.reg.min.x, &m.reg.min.y, &m.reg.min.z, &m.reg.max.x, &m.reg.max.y, &m.reg.max.z,
               &texturePath[0], &colora[0], &colora[1], &colora[2], &colord[0], &colord[1], &colord[2],
               &colors[0], &colors[1], &colors[2], &alpha, &material[0]);

        /// if texture is not only single color then load it
        if((texturePath[0] != '*') && (texturePath[0] != '('))
            m.texture2D = getTexture(f->path() + texturePath);
        /// create color texture
        else
            m.texture2D = getTexture(colord[0], colord[1], colord[2]);

        int cursor = 0;
        m.dynamic = false;
        m.filter = 0;
        m.touchable = false;
        if (m.texture2D->transparent)
            m.material = getShader("standart_alpha");
        else
            m.material = getShader("standart");

        /// get material parameters
        while(true)
        {
            if (material[cursor] == '!')
            {
                m.touchable = true;
                cursor++;
            } else if (material[cursor] == '$')
            {
                m.dynamic = true;
                cursor++;
            } else if (material[cursor] == '#')
            {
                cursor++;
                m.filter = material[cursor] - '0';
                cursor++;
            } else if (material[cursor] == '%')
            {
                cursor++;
                m.texture2D->transparent = false;
                char* shadername = new char[strlen(material) - cursor + 1];
                for (unsigned int j = cursor; j < strlen(material); j++)
                {
                    shadername[j - cursor] = material[j];
                    if (material[j] == '/')
                    {
                        shadername[j - cursor] = '\000';
                        break;
                    }
                }
                shadername[strlen(material) - cursor] = '\000';
                m.material = getShader(shadername);
                delete[] shadername;
                break;
            } else
                break;
        }

        /// prepare model arrays
        int count = f->scandec();
        float v[9];
        float n[9];
        float t[6];
        for (int j = 0; j < count; j++)
        {
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

void model::culling()
{
    for (int j = models.size() - 1; j >= 0; j--)
    {
        std::map<id3d, bool> used;
        if ((models[j].filter == 0) && !models[j].dynamic && !models[j].touchable)
        {
            model3d* m = &models[j];
            long v = 0;
            long t = 0;
            id3d id;
            glm::vec3 center;
            glm::vec3 va, vb, vc;
            glm::vec3 na, nb, nc;
            glm::vec2 ta, tb, tc;
            for (unsigned int i = 0; i < m->vertices.size() / 9; i++, v += 9, t += 6)
            {
                //get geometry
                va = glm::vec3(m->vertices[v + 0] + m->reg.min.x,
                               m->vertices[v + 1] + m->reg.min.y,
                               m->vertices[v + 2] + m->reg.min.z);
                vb = glm::vec3(m->vertices[v + 3] + m->reg.min.x,
                               m->vertices[v + 4] + m->reg.min.y,
                               m->vertices[v + 5] + m->reg.min.z);
                vc = glm::vec3(m->vertices[v + 6] + m->reg.min.x,
                               m->vertices[v + 7] + m->reg.min.y,
                               m->vertices[v + 8] + m->reg.min.z);
                na = glm::vec3(m->normals[v + 0], m->normals[v + 1], m->normals[v + 2]);
                nb = glm::vec3(m->normals[v + 3], m->normals[v + 4], m->normals[v + 5]);
                nc = glm::vec3(m->normals[v + 6], m->normals[v + 7], m->normals[v + 8]);
                ta = glm::vec2(m->coords[t + 0], m->coords[t + 1]);
                tb = glm::vec2(m->coords[t + 2], m->coords[t + 3]);
                tc = glm::vec2(m->coords[t + 4], m->coords[t + 5]);
                //get id
                center = (va + vb + vc) / 3.0f;
                id.x = center.x / CULLING_DST;
                id.y = center.y / CULLING_DST;
                id.z = center.z / CULLING_DST;
                if (used.find(id) == used.end())
                {
                    model3d mod;
                    mod.dynamic = false;
                    mod.material = models[j].material;
                    mod.texture2D = models[j].texture2D;
                    v3d[id].push_back(mod);
                    used[id] = true;
                }
                //put triangle into structure
                model3d* mod = &v3d[id][v3d[id].size() - 1];
                mod->vertices.push_back(va.x);
                mod->vertices.push_back(va.y);
                mod->vertices.push_back(va.z);
                mod->vertices.push_back(vb.x);
                mod->vertices.push_back(vb.y);
                mod->vertices.push_back(vb.z);
                mod->vertices.push_back(vc.x);
                mod->vertices.push_back(vc.y);
                mod->vertices.push_back(vc.z);
                mod->normals.push_back(na.x);
                mod->normals.push_back(na.y);
                mod->normals.push_back(na.z);
                mod->normals.push_back(nb.x);
                mod->normals.push_back(nb.y);
                mod->normals.push_back(nb.z);
                mod->normals.push_back(nc.x);
                mod->normals.push_back(nc.y);
                mod->normals.push_back(nc.z);
                mod->coords.push_back(ta.s);
                mod->coords.push_back(ta.t);
                mod->coords.push_back(tb.s);
                mod->coords.push_back(tb.t);
                mod->coords.push_back(tc.s);
                mod->coords.push_back(tc.t);
            }
            models.erase(models.begin() + j);
        }
    }
}
