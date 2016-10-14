///----------------------------------------------------------------------------------------
/**
 * \file       physics.h
 * \author     Vonasek Lubos
 * \date       2014/12/30
 * \brief      Physical model for scene, it detects collision and also calculate collision
 *             reactions. Also applies car physical and visual state.
**/
///----------------------------------------------------------------------------------------

#ifndef PHYSICS_H
#define PHYSICS_H

#include "engine/car.h"

/**
 * @brief The physics interface
 */
class physics
{
public:
    bool active;    ///< Information if scene is active
    bool locked;    ///< Locker for car controling

    /**
     * @brief physics desructor
     */
    virtual ~physics() {}

    /**
     * @brief addCar adds car into physical model
     * @param c is car instance
     */
    virtual void addCar(car* c) = 0;

    /**
     * @brief addModel adds model into physical model
     * @param m is 3D model for physical model
     * @param id is 3d position index
     */
    virtual void addModel(model *m, id3d id) = 0;

    /**
     * @brief getTransform counts OpenGL matrix of transformation
     * @param index is index of object
     * @param m is float[16] for output
     * @param id is 3d position index
     */
    virtual void getTransform(int index, float* m, id3d id) = 0;

    /**
     * @brief removeModel removes model from physical engine
     * @param id is 3d position index
     */
    virtual void removeModel(id3d id) = 0;

    /**
     * @brief resetCar updates car state
     * @param c is instance of car
     */
    virtual void resetCar(car* c, bool total = true) = 0;

    /**
     * @brief updateCar updates car state
     * @param c is instance of car
     */
    virtual void updateCar(car* c) = 0;

    /**
     * @brief updateWorld updates world state
     */
    virtual void updateWorld() = 0;
};

#endif // PHYSICS_H
