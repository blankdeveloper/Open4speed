//----------------------------------------------------------------------------------------
/**
 * \file       switch.cpp
 * \author     Vonasek Lubos
 * \date       2014/01/05
 * \brief      This utility switches between components. Switch depends on configuration
 *             file.
*/
//----------------------------------------------------------------------------------------

#include "stdafx.h"

/**
 * @brief getInput gets input controller
 * @return input controller
 */
input* getInput() {
    if (strcmp(inputController, "keyboard") == 0) {
        logi("Init keyboard", "");
        return new keyboard();
    }
    loge("Input controller not found:", inputController);
    return 0;
}

/**
 * @brief getModel gets model
 * @param filename is path and name of file to load
 * @return instance of model
 */
model* getModel(const char* filename, bool lightmaps) {
    logi("Load model:", filename);
    if (strcmp(getExtension(filename), "o4s") == 0) {
        return new modelo4s(filename, lightmaps);
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
    if (strcmp(physicalEngine, "bullet") == 0) {
        return new bullet(m);
    }
    loge("Physical engine not found:", physicalEngine);
    return 0;
}

/**
 * @brief getRenderer gets renderer
 * @return renderer instance
 */
renderer* getRenderer() {
    logi("Init renderer ",screenRenderer);
    if (strcmp(screenRenderer, "glsl") == 0) {
        return new gles20();
    }
    loge("Renderer not found:", screenRenderer);
    return 0;
}

/**
 * @brief getShader compile shader from file
 * @param name is shader filename
 * @return instance of shader
 */
shader* getShader(const char* name) {
    /// find previous instance
    for (unsigned int i = 0; i < shaders->size(); i++) {
        if (strcmp((*shaders)[i]->shadername, name) == 0) {
            return (*shaders)[i];
        }
    }

    char* vert = new char[256];
    strcpy(vert, "shaders/");
    strcat(vert, name);
    strcat(vert, ".vert");
    char* frag = new char[256];
    strcpy(frag, "shaders/");
    strcat(frag, name);
    strcat(frag, ".frag");

#ifdef ZIP_ARCHIVE
    std::vector<char*> *vert_atributes = getFullList(zip_fopen(APKArchive, prefix(vert), 0));
    std::vector<char*> *frag_atributes = getFullList(zip_fopen(APKArchive, prefix(frag), 0));
#else
    std::vector<char*> *vert_atributes = getFullList(fopen(prefix(vert), "r"));
    std::vector<char*> *frag_atributes = getFullList(fopen(prefix(frag), "r"));
#endif

    /// create shader from code
    if (strcmp(screenRenderer, "glsl") == 0) {
        shader* instance = new glsl(vert_atributes, frag_atributes);
        strcpy(instance->shadername, name);
        shaders->push_back(instance);
        return instance;
    }
    loge("Renderer incompatible shader", "");
    return 0;
}

/**
 * @brief getSound gets sound
 * @param filename is name of file to load
 * @param loop is true for sound looping
 * @param channels is amount of channels
 * @return sound instance
 */
sound* getSound(const char* filename, bool loop, int channels) {
    logi("Load sound file:", filename);
#ifdef ANDROID
    return new soundpool(filename, loop, channels);
#else
    return new fmodapi(filename, loop, channels);
#endif
}

/**
 * @brief getTexture gets texture
 * @param filename is filename of texture
 * @param alpha is amount of blending
 * @return texture instance
 */
texture* getTexture(const char* filename, float alpha) {
    /// find previous instance
    for (unsigned int i = 0; i < textures->size(); i++) {
        if (strcmp((*textures)[i]->texturename, filename) == 0) {
          if ((*textures)[i]->instanceCount > 0) {
              (*textures)[i]->instanceCount++;
              return (*textures)[i];
          }
        }
    }

    /// create new instance
    logi("Load texture:", filename);
    if (strcmp(getExtension(filename), "png") == 0) {
      texture* instance = new pngloader(filename, alpha);
      instance->instanceCount = 1;
      strcpy(instance->texturename, filename);
      textures->push_back(instance);
      return instance;
    } else if (getExtension(filename)[0] == 'p') {
        texture* instance = new pxxloader(filename, alpha);
        instance->instanceCount = 1;
        strcpy(instance->texturename, filename);
        textures->push_back(instance);
        return instance;
    }

    logi("File is not supported:",filename);
    return gray;
}
