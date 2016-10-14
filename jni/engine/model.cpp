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
 * @param mtlLoader is instance of object for loading materials
 */
model::model(std::string filename, materialLoader* mtlLoader)
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
            m.texture2D = mtlLoader->getTexture(f->path() + texturePath);
        /// create color texture
        else
            m.texture2D = mtlLoader->getTexture(colord[0], colord[1], colord[2]);

        int cursor = 0;
        m.dynamic = false;
        m.filter = 0;
        m.touchable = false;
        if (m.texture2D->transparent)
            m.material = mtlLoader->getShader("standart_alpha");
        else
            m.material = mtlLoader->getShader("standart");

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
                m.material = mtlLoader->getShader(shadername);
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
