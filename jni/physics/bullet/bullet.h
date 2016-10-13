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
#include "physics/bullet/GL_ShapeDrawer.h"

/**
 * @brief The bullet physics implementation class
 */
class bullet:public physics {
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
    GL_ShapeDrawer* m_shapeDrawer;

    /**
     * Geometry objects
     */
    std::vector<btRigidBody*> dynamicObjects;
    std::vector<btCollisionObject*> staticObjects;
    std::vector<btTriangleMesh*> staticMeshes;
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
     * @param center is model translation
     */
    void addModel(model *m);

    /**
     * @brief getTransform counts OpenGL matrix of transformation
     * @param index is index of object
     * @return transformation matrix
     */
    void getTransform(int index, float* m);

    void render();

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
};

#endif // BULLET_H
