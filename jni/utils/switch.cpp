//----------------------------------------------------------------------------------------
/**
 * \file       switch.cpp
 * \author     Vonasek Lubos
 * \date       2014/01/05
 * \brief      This utility switches between components. Switch depends on configuration
 *             file.
*/
//----------------------------------------------------------------------------------------

#include "input/keyboard.h"
#include "loaders/modelo4s.h"
#include "loaders/pngloader.h"
#include "loaders/rgb.h"
#include "physics/bullet.h"
#include "renderers/opengl/gles20.h"
#include "renderers/opengl/glsl.h"
#include "renderers/opengl/gltexture.h"
#include "renderers/opengl/glvbo.h"
#include "sound/soundpool.h"
#include "utils/io.h"
#include "utils/switch.h"
#include "common.h"

#define BULLET
#define GLES20

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
model* getModel(std::string filename, bool gpu) {
    logi("Load model:", filename);
    if (strcmp(getExtension(filename).c_str(), "o4s") == 0) {
        return new modelo4s(filename, gpu);
    }
    loge("File is not supported:", filename);
    return 0;
}

/**
 * @brief getPhysics gets physical engine
 * @param m is 3D model of scene
 * @return physical engine
 */
physics* getPhysics(model *m) {
    logi("Init physical engine","");
#ifdef BULLET
    return new bullet(m);
#endif
    exit(1);
}

/**
 * @brief getRenderer gets renderer
 * @return renderer instance
 */
renderer* getRenderer(int w, int h) {
    logi("Init renderer","");
#ifdef GLES20
    return new gles20(w, h);
#endif
    exit(1);
}

/**
 * @brief getShader compile shader from file
 * @param name is shader filename
 * @return instance of shader
 */
shader* getShader(std::string name) {
    /// find previous instance
    for (unsigned int i = 0; i < shaders.size(); i++) {
        if (strcmp(shaders[i]->shadername, name.c_str()) == 0) {
            return shaders[i];
        }
    }

    char filename[256];
    strcpy(filename, "shaders/");
    strcat(filename, name.c_str());
    strcat(filename, ".glsl");
    logi("Load shader:", filename);

    std::vector<std::string> vert_atributes = getList("VERT", filename);
    std::vector<std::string> frag_atributes = getList("FRAG", filename);

    /// create shader from code
#ifdef GLES20
    shader* instance = new glsl(vert_atributes, frag_atributes);
    strcpy(instance->shadername, name.c_str());
    shaders.push_back(instance);
    return instance;
#endif
    exit(1);
}

/**
 * @brief getSound gets sound
 * @param filename is name of file to load
 * @param loop is true for sound looping
 * @param channels is amount of channels
 * @return sound instance
 */
sound* getSound(std::string filename, bool loop, int channels) {
    logi("Load sound file:", filename);
    return new soundpool(filename, loop, channels);
}

/**
 * @brief getTexture gets texture
 * @param filename is filename of texture
 * @param alpha is amount of blending
 * @return texture instance
 */
texture* getTexture(std::string filename, float alpha) {
    /// find previous instance
    for (unsigned int i = 0; i < textures.size(); i++) {
        if (strcmp(textures[i]->texturename, filename.c_str()) == 0) {
          if (textures[i]->instanceCount > 0) {
              textures[i]->instanceCount++;
              return textures[i];
          }
        }
    }

    /// create new instance
    logi("Load texture:", filename);
#ifdef GLES20
    if (strcmp(getExtension(filename).c_str(), "png") == 0) {
      texture* instance = new gltexture(loadPNG(filename), alpha);
      instance->instanceCount = 1;
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
            anim.push_back(new gltexture(loadPNG(file), alpha));
            anim[anim.size() - 1]->instanceCount = 1;
            anim[anim.size() - 1]->texturename[0] = '\0';
        }

        texture* instance = new gltexture(anim, alpha);
        instance->instanceCount = 1;
        strcpy(instance->texturename, filename.c_str());
        textures.push_back(instance);
        return instance;
    }
#endif
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
#ifdef GLES20
    return new gltexture(*createRGB(1, 1, r, g, b), alpha);
#endif
    exit(1);
}

/**
 * @brief glvbo creates VBO from data
 * @param size is amount of vertices
 * @param vertices is vertices array
 * @param normals is normals array
 * @param coords is texture coords array
 * @param tnormals is triangle normals array
 */
vbo* getVBO(int size, float* vertices, float* normals, float* coords, float* tnormals) {
#ifdef GLES20
    return new glvbo(size, vertices, normals, coords, tnormals);
#endif
    exit(1);
}
