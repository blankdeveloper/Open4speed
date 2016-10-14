///----------------------------------------------------------------------------------------
/**
 * \file       scene.h
 * \author     Vonasek Lubos
 * \date       2016/10/14
 * \brief      Class for holding and managing scene data
**/
///----------------------------------------------------------------------------------------

#ifndef SWITCH_H
#define SWITCH_H

#include <string>
#include "engine/model.h"
#include "interfaces/input.h"
#include "interfaces/materialLoader.h"
#include "interfaces/physics.h"
#include "interfaces/renderer.h"

struct Dynamic
{
    float* vertices;
    float* coords;
    int count;
    int frame;
};

#define WATER_EFF_LENGTH 5

/**
 * @brief The model class
 */
class scene : materialLoader
{
public:

    /**
     * @brief Constructor loads scene from Open4speed config file
     * @param filename is scene configuration file (o4scfg)
     */
    scene(std::string filename);

    /**
     * @brief model destructor
     */
    ~scene();

    /**
     * @brief addCar adds car into scene
     * @param c is instance of new car
     */
    void addCar(car* c);

    /**
     * @brief getCar gets car from scene
     * @param index is index of car
     * @return car instance
     */
    car* getCar(int index);

    /**
     * @brief getCarCount gets count of cars in scene
     * @return count of cars
     */
    unsigned int getCarCount();

    /**
     * @brief getModel gets model
     * @param filename is path and name of file to load
     * @return instance of model
     */
    model* getModel(std::string filename);

    /**
     * @brief getShader compile shader from file
     * @param name is shader filename
     * @return instance of shader
     */
    shader* getShader(std::string name);

    /**
     * @brief getTexture gets texture
     * @param filename is filename of texture
     * @return texture instance
     */
    texture* getTexture(std::string filename);

    /**
     * @brief getTexture gets texture
     * @param r is amount of red from 0 to 1
     * @param g is amount of green from 0 to 1
     * @param b is amount of blue from 0 to 1
     * @return texture instance
     */
    texture* getTexture(float r, float g, float b);

    /**
     * @brief initRenderer inits renderer
     * @param w is screen width
     * @param h is screen height
     * @param a is screen aliasing(reducing resolution)
     */
    void initRenderer(int w, int h, float a);

    /**
     * @brief render shows scene
     * @param cameraCar is index of car which should be traced by camera
     */
    void render(int cameraCar);

    /**
     * @brief resetCar resets car if it is on roof
     * @param index is index of car to reset
     * @param total is true to also return car on road
     */
    void resetCar(int index, bool total) { physic->resetCar(getCar(index), total); }

    /**
     * @brief setPhysicsLocked lock/unlock physics movement
     * @param locked is true to lock movement
     */
    void setPhysicsLocked(bool locked) { physic->locked = locked; }

    /**
     * @brief update updates scene physics
     */
    void update();

private:
    /**
     * @brief The game resources
     */
    std::vector<car*> cars;                   ///< All cars in scene instances
    std::map<std::string, model*> models;     ///< Models storage
    std::map<std::string, shader*> shaders;   ///< Shaders storage
    std::map<std::string, texture*> textures; ///< Textures storage
    input *controller;                        ///< Instance of control device
    physics *physic;                          ///< Physical engine instance
    renderer *xrenderer;                      ///< Renderer instance
    std::string shaderPath;                   ///< Path to shader files

    /**
     * @brief The scene state
     */
    float aspect;                             ///< Screen aspect
    int currentFrame;                         ///< Frame index
    float direction;                          ///< Camera direction
    int viewDistance;                         ///< Camera view distance
    model *skydome;                           ///< Skydome model
    model *trackdata;                         ///< Track first model
    model *water;                             ///< Water effect model
    Dynamic eff[WATER_EFF_LENGTH];            ///< 3D water effect object
};

#endif // SWITCH_H
