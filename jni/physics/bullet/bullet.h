///----------------------------------------------------------------------------------------
/**
 * \file       bullet.h
 * \author     Vonasek Lubos
 * \date       2014/12/30
 * \brief      Physical model for scene, it detects collision and also calculate collision
 *             reactions. Also applies car physical and visual state.
**/
///----------------------------------------------------------------------------------------

#ifndef BULLET_H
#define BULLET_H

#include <BulletDynamics/Vehicle/btRaycastVehicle.h>
#include <btBulletDynamicsCommon.h>
#include "interfaces/physics.h"

/**
 * @brief The bullet physics implementation class
 */
class bullet:public physics
{
public:

    /**
     * Bullet physics objects
     */
    btDynamicsWorld* m_dynamicsWorld;
    btCollisionConfiguration* m_collisionConfiguration;
    btCollisionDispatcher* m_dispatcher;
    btBroadphaseInterface* m_overlappingPairCache;
    btConstraintSolver* m_constraintSolver;
    btVehicleRaycaster* m_vehicleRayCaster;

    /**
     * Geometry objects
     */
    std::map<id3d, std::vector<btRigidBody*> > dynamicObjects;
    std::map<id3d, std::vector<btCollisionObject*> > staticObjects;
    std::map<id3d, std::vector<btTriangleMesh*> > staticMeshes;
    std::vector<btRaycastVehicle*> vehicles;

    /**
     * @brief Construct physical model
     */
    bullet();

    /**
     * @brief bullet destructor
     */
    virtual ~bullet();

    /**
     * @brief addCar adds car into physical model
     * @param c is car instance
     */
    void addCar(car* c);

    /**
     * @brief addModel adds model into physical model
     * @param m is 3D model for physical model
     * @param id is 3d position index
     */
    void addModel(model *m, id3d id);

    /**
     * @brief getTransform counts OpenGL matrix of transformation
     * @param index is index of object
     * @param m is float[16] for output
     * @param id is 3d position index
     */
    void getTransform(int index, float* m, id3d id);

    /**
     * @brief removeModel removes model from physical engine
     * @param id is 3d position index
     */
    void removeModel(id3d id);

    /**
     * @brief resetCar updates car state
     * @param c is instance of car
     */
    void resetCar(car* c, bool total = true);

    /**
     * @brief updateCar updates car state
     * @param c is instance of car
     */
    void updateCar(car* c);

    /**
     * @brief updateWorld updates world state
     */
    void updateWorld();

private:
    static pthread_mutex_t mutex;  ///< Lock for multithreading
};

#endif // BULLET_H
