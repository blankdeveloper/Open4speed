///----------------------------------------------------------------------------------------
/**
 * \file       switch.cpp
 * \author     Vonasek Lubos
 * \date       2014/12/31
 * \brief      This utility switches between components. Switch depends on configuration
 *             file.
**/
///----------------------------------------------------------------------------------------


#include "engine/io.h"
#include "engine/model.h"
#include "engine/switch.h"
#include "input/keyboard.h"
#include "physics/bullet/bullet.h"
#include "renderers/opengl/gles20.h"
#include "renderers/opengl/glsl.h"
#include "renderers/opengl/gltexture.h"

/**
 * @brief The game resources
 */
std::vector<car*> cars;                   ///< All cars in scene instances
std::map<std::string, model*> models;     ///< Models storage
std::map<std::string, shader*> shaders;   ///< Shaders storage
std::map<std::string, texture*> textures; ///< Textures storage
physics *physic = 0;                      ///< Physical engine instance
renderer *xrenderer = 0;                  ///< Renderer instance
std::string shaderPath;                   ///< Path to shader files

void clearMediaStorage()
{
    while (!cars.empty())
    {
        delete cars[cars.size() - 1];
        cars.pop_back();
    }
    for (std::map<std::string, model*>::const_iterator it = models.begin(); it != models.end(); ++it)
        delete it->second;
    models.clear();
    for (std::map<std::string, shader*>::const_iterator it = shaders.begin(); it != shaders.end(); ++it)
        delete it->second;
    shaders.clear();
    for (std::map<std::string, texture*>::const_iterator it = textures.begin(); it != textures.end(); ++it)
        delete it->second;
    textures.clear();

    if (physic)
        delete physic;
    physic = 0;
    if (xrenderer)
        delete xrenderer;
    xrenderer = 0;
}

/**
 * @brief addCar adds car into scene
 * @param c is instance of new car
 */
void addCar(car* c)
{
    cars.push_back(c);
}

/**
 * @brief getCar gets car from scene
 * @param index is index of car
 * @return car instance
 */
car* getCar(int index)
{
    return cars[index];
}

/**
 * @brief getCarCount gets count of cars in scene
 * @return count of cars
 */
unsigned int getCarCount()
{
    return cars.size();
}

/**
 * @brief setShaderPath sets path of shaders
 * @param path is relative path to runable file
 */
void setShaderPath(std::string path)
{
    shaderPath = path;
}

/**
 * @brief getInput gets input controller
 * @return input controller
 */
input* getInput()
{
    return new keyboard();
}

/**
 * @brief getModel gets model
 * @param filename is path and name of file to load
 * @return instance of model
 */
model* getModel(std::string filename)
{
    /// find previous instance
    filename = fixName(filename);
    if (models.find(filename) != models.end())
        return models[filename];

    /// create new instance
    if (strcmp(getExtension(filename).c_str(), "o4s") == 0)
    {
        model* instance = new model(filename);
        models[filename] = instance;
        return instance;
    }
    loge("File is not supported:", filename);
    return 0;
}

/**
 * @brief getPhysics gets physical engine
 * @return physical engine
 */
physics* getPhysics()
{
    if (!physic)
    {
        logi("Init physical engine","");
        physic = new bullet();
    }
    return physic;
}

/**
 * @brief getRenderer gets renderer
 * @return renderer instance
 */
renderer* getRenderer()
{
    if (!xrenderer)
    {
        logi("Init renderer","");
        xrenderer = new gles20();
    }
    return xrenderer;
}

/**
 * @brief getShader compile shader from file
 * @param name is shader filename
 * @return instance of shader
 */
shader* getShader(std::string name)
{
    char filename[1024];
    strcpy(filename, shaderPath.c_str());
    strcat(filename, name.c_str());
    strcat(filename, ".glsl");

    /// find previous instance
    name = fixName(name);
    if (shaders.find(name) != shaders.end())
        return shaders[name];

    std::vector<std::string> vert_atributes = getList("VERT", filename);
    std::vector<std::string> frag_atributes = getList("FRAG", filename);

    /// create shader from code
    shader* instance = new glsl(vert_atributes, frag_atributes);
    shaders[name] = instance;
    return instance;
}

/**
 * @brief getTexture gets texture
 * @param filename is filename of texture
 * @return texture instance
 */
texture* getTexture(std::string filename)
{
    filename = fixName(filename);

    /// find previous instance
    if (textures.find(filename) != textures.end())
        return textures[filename];

    /// create new instance
    if (strcmp(getExtension(filename).c_str(), "png") == 0)
    {
      texture* instance = new gltexture(texture::loadPNG(getFile(filename)));
      textures[filename] = instance;
      return instance;
    } else if (getExtension(filename)[0] == 'p')
    {
        /// get animation frame count
        std::string ext = getExtension(filename);
        int count = (ext[1] - '0') * 10 + ext[2] - '0';
        char file[255];
        strcpy(file, filename.c_str());

        /// load all sequence images
        std::vector<texture*> anim;
        for (int i = 0; i <= count; i++)
        {
            file[strlen(file) - 1] = i % 10 + '0';
            file[strlen(file) - 2] = i / 10 + '0';
            texture* instance = new gltexture(texture::loadPNG(getFile(file)));
            anim.push_back(instance);
        }

        texture* instance = new gltexture(anim);
        textures[filename] = instance;
        return instance;
    }
    loge("Unsupported texture", filename);
    exit(1);
}

/**
 * @brief getTexture gets texture
 * @param r is amount of red from 0 to 1
 * @param g is amount of green from 0 to 1
 * @param b is amount of blue from 0 to 1
 * @return texture instance
 */
texture* getTexture(float r, float g, float b)
{
    char filename[256];
    sprintf(filename, "%d %d %d", (int)(r * 255.0f), (int)(g * 255.0f), (int)(b * 255.0f));

    /// find previous instance
    if (textures.find(filename) != textures.end())
        return textures[filename];

    texture* instance = new gltexture(texture::createRGB(1, 1, r, g, b));
    textures[filename] = instance;
    return instance;
}
