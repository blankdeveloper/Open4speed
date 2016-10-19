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
    for (unsigned int i = 0; i < models.size(); i++)
    {
        models[i].material->instanceCount--;
        models[i].texture2D->instanceCount--;
        if (models[i].vertices)
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
 * @param mtlLoader is instance of object for loading materials
 */
model::model(std::string filename, materialLoader* mtlLoader)
{
    /// open file
    file* f = getFile(filename);

    /// get model dimensions
    toDelete = false;
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
                m.material->instanceCount--;
                m.material = mtlLoader->getShader(shadername);
                delete[] shadername;
                break;
            } else
                break;
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
    delete f;
}
