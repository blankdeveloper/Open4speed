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
#include "engine/textures.hpp"
#include "files/extfile.h"
#include "files/zipfile.h"
#include "input/keyboard.h"
#include "physics/bullet/bullet.h"
#include "renderers/opengl/gles20.h"
#include "renderers/opengl/glsl.h"
#include "renderers/opengl/gltexture.h"
#include "renderers/opengl/glvbo.h"

/**
 * @brief The game resources
 */
std::vector<car*> cars;         ///< All cars in scene instances
std::vector<model*> models;     ///< Shaders storage
std::vector<shader*> shaders;   ///< Shaders storage
std::vector<texture*> textures; ///< Textures storage
physics *physic = 0;            ///< Physical engine instance
renderer *xrenderer = 0;        ///< Renderer instance
zip *APKArchive = 0;            ///< Access to APK archive
std::string shaderPath;         ///< Path to shader files

void clearMediaStorage() {
    while (!cars.empty()) {
        delete cars[cars.size() - 1];
        cars.pop_back();
    }
    while (!models.empty()) {
        delete models[models.size() - 1];
        models.pop_back();
    }
    while (!shaders.empty()) {
        delete shaders[shaders.size() - 1];
        shaders.pop_back();
    }
    while (!textures.empty()) {
        delete textures[textures.size() - 1];
        textures.pop_back();
    }
    delete physic;
    delete xrenderer;
}

/**
 * @brief addCar adds car into scene
 * @param c is instance of new car
 */
void addCar(car* c) {
    cars.push_back(c);
}

/**
 * @brief getCar gets car from scene
 * @param index is index of car
 * @return car instance
 */
car* getCar(int index) {
    return cars[index];
}

/**
 * @brief getCarCount gets count of cars in scene
 * @return count of cars
 */
unsigned int getCarCount() {
    return cars.size();
}

/**
 * @brief setShaderPath sets path of shaders
 * @param path is relative path to runable file
 */
void setShaderPath(std::string path) {
    shaderPath = path;
}

file* getFile(std::string filename) {
    filename = fixName(filename);
    if (!APKArchive) {
        if (filename[0] == '#')
            filename = filename.substr(1, filename.length() - 1);
        logi("Opening file:", filename);
        return new extfile(filename);
    } else {
        logi("Opening file:", filename);
        if (filename[0] == '#')
            return new zipfile(filename.substr(1, filename.length() - 1), APKArchive);
        else
            return new extfile(filename);
    }
}

/**
 * @brief getInput gets input controller
 * @return input controller
 */
input* getInput() {
    return new keyboard();
}

/**
 * @brief getModel gets model
 * @param filename is path and name of file to load
 * @return instance of model
 */
model* getModel(std::string filename) {

    /// find previous instance
    filename = fixName(filename);
    for (unsigned int i = 0; i < models.size(); i++)
        if (strcmp(models[i]->modelname, filename.c_str()) == 0)
            return models[i];

    /// create new instance
    if (strcmp(getExtension(filename).c_str(), "o4s") == 0) {
        model* instance = new model(filename);
        strcpy(instance->modelname, filename.c_str());
        models.push_back(instance);
        return instance;
    }
    loge("File is not supported:", filename);
    return 0;
}

/**
 * @brief getPhysics gets physical engine
 * @return physical engine
 */
physics* getPhysics() {
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
renderer* getRenderer() {
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
shader* getShader(std::string name) {

    /// find previous instance
    name = fixName(name);
    for (unsigned int i = 0; i < shaders.size(); i++) {
        if (strcmp(shaders[i]->shadername, name.c_str()) == 0) {
            return shaders[i];
        }
    }

    char filename[256];
    strcpy(filename, shaderPath.c_str());
    strcat(filename, name.c_str());
    strcat(filename, ".glsl");

    std::vector<std::string> vert_atributes = getList("VERT", filename);
    std::vector<std::string> frag_atributes = getList("FRAG", filename);

    /// create shader from code
    shader* instance = new glsl(vert_atributes, frag_atributes);
    strcpy(instance->shadername, name.c_str());
    shaders.push_back(instance);
    return instance;
}

/**
 * @brief getTexture gets texture
 * @param filename is filename of texture
 * @param alpha is amount of blending
 * @return texture instance
 */
texture* getTexture(std::string filename, float alpha) {
    filename = fixName(filename);

    /// find previous instance
    for (unsigned int i = 0; i < textures.size(); i++)
        if (strcmp(textures[i]->texturename, filename.c_str()) == 0)
            return textures[i];

    /// create new instance
    if (strcmp(getExtension(filename).c_str(), "png") == 0) {
      texture* instance = new gltexture(loadPNG(filename), alpha);
      strcpy(instance->texturename, filename.c_str());
      textures.push_back(instance);
      return instance;
    } else if (getExtension(filename)[0] == 'p') {

        /// get animation frame count
        std::string ext = getExtension(filename);
        int count = (ext[1] - '0') * 10 + ext[2] - '0';
        char file[255];
        strcpy(file, filename.c_str());

        /// load all sequence images
        std::vector<texture*> anim;
        for (int i = 0; i <= count; i++) {
            file[strlen(file) - 1] = i % 10 + '0';
            file[strlen(file) - 2] = i / 10 + '0';
            texture* instance = new gltexture(loadPNG(file), alpha);
            strcpy(instance->texturename, file);
            anim.push_back(instance);
        }

        texture* instance = new gltexture(anim, alpha);
        strcpy(instance->texturename, filename.c_str());
        textures.push_back(instance);
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
 * @param alpha is amount of blending
 * @return texture instance
 */
texture* getTexture(float r, float g, float b, float alpha) {
    texture* instance = new gltexture(createRGB(1, 1, r, g, b), alpha);
    sprintf(instance->texturename, "%f %f %f", r, g , b);
    textures.push_back(instance);
    return instance;
}

/**
 * @brief glvbo creates VBO from data
 * @param size is amount of vertices
 * @param vertices is vertices array
 * @param normals is normals array
 * @param coords is texture coords array
 */
vbo* getVBO(int size, float* vertices, float* normals, float* coords) {
    return new glvbo(size, vertices, normals, coords);
}

/**
 * @brief setZip sets APK archive object
 * @param path is path of APK
 */
void setZip(std::string path) {
    APKArchive = zip_open(path.c_str(), 0, NULL);
}
