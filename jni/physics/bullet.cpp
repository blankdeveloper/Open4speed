//----------------------------------------------------------------------------------------
/**
 * \file       bullet.cpp
 * \author     Vonasek Lubos
 * \date       2014/01/05
 * \brief      Physical model for scene, it detects collision and also calculate collision
 *             reactions. Also applies car physical and visual state.
*/
//----------------------------------------------------------------------------------------

#include "physics/bullet.h"
#include "utils/io.h"
#include "utils/math.h"
#include "common.h"

float eyemat[16] = {
    1,0,0,0,
    0,1,0,0,
    0,0,1,0,
    0,0,0,1
};

/**
 * @brief Construct physical model
 * @param m is 3D model for physical model
 */
bullet::bullet(model *m) {

    /// Get configuration
    std::vector<char*> *atributes = getList("BULLETCFG");
    brakeAspect = getConfig("brakeAspect", atributes);
    engineMaxSpeed = getConfig("engineMaxSpeed", atributes);
    gasAspect = getConfig("gasAspect", atributes);
    gravitation = getConfig("gravitation", atributes);
    overSpeedBraking = getConfig("overSpeedBraking", atributes);
    rollInfluence = getConfig("rollInfluence", atributes);
    skinFriction = getConfig("skinFriction", atributes);
    speedDecrease = getConfig("speedDecrease", atributes);
    steeringAspect = getConfig("steeringAspect", atributes);
    steeringSpeedDependency = getConfig("steeringSpeedDependency", atributes);
    suspensionCompression = getConfig("suspensionCompression", atributes);
    suspensionDamping = getConfig("suspensionDamping", atributes);
    suspensionStiffness = getConfig("suspensionStiffness", atributes);
    underSpeedBraking = getConfig("underSpeedBraking", atributes);
    vehicleMassAspect = getConfig("vehicleMassAspect", atributes);
    vehicleStep = getConfig("vehicleStep", atributes);
    wheelFriction = getConfig("wheelFriction", atributes);
    worldLimit = getConfig("worldLimit", atributes);
    worldStep = getConfig("worldStep", atributes);
    worldSubStep = getConfig("worldSubStep", atributes);
    delete atributes;
    locked = true;

    /// init engine
    m_collisionConfiguration = new btDefaultCollisionConfiguration();
    m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
    btVector3 worldMin(-worldLimit,-worldLimit,-worldLimit);
    btVector3 worldMax(worldLimit,worldLimit,worldLimit);
    m_overlappingPairCache = new btAxisSweep3(worldMin,worldMax);
    m_constraintSolver = new btSequentialImpulseConstraintSolver();
    m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher,m_overlappingPairCache,m_constraintSolver,m_collisionConfiguration);
    m_dynamicsWorld->setGravity(btVector3(0,-gravitation,0));

    /// Create scene
    addModel(m);
}

/**
 * @brief bullet destructor
 */
bullet::~bullet() {
    while(!m_vehicle.empty()) {
        delete m_vehicle[0];
        m_vehicle.erase(m_vehicle.begin());
    }
    while(!bodies.empty()) {
        delete bodies[0];
        bodies.erase(bodies.begin());
    }
    while(!shapes.empty()) {
        delete shapes[0];
        shapes.erase(shapes.begin());
    }
    while(!compounds.empty()) {
        delete compounds[0];
        compounds.erase(compounds.begin());
    }
    while(!meshes.empty()) {
        delete meshes[0];
        meshes.erase(meshes.begin());
    }
    while(!levelShapes.empty()) {
        delete levelShapes[0];
        levelShapes.erase(levelShapes.begin());
    }
    while(!bodies2.empty()) {
        delete bodies2[0];
        bodies2.erase(bodies2.begin());
    }
    while(!chassisShapes.empty()) {
        delete chassisShapes[0];
        chassisShapes.erase(chassisShapes.begin());
    }
    while(!m_vehicleRayCasters.empty()) {
        delete m_vehicleRayCasters[0];
        m_vehicleRayCasters.erase(m_vehicleRayCasters.begin());
    }
    while(!m_RaycastVehicles.empty()) {
        //delete m_RaycastVehicles[0];
        m_RaycastVehicles.erase(m_RaycastVehicles.begin());
    }

    delete m_collisionConfiguration;
    delete m_dispatcher;
    delete m_overlappingPairCache;
    delete m_constraintSolver;
    //delete m_dynamicsWorld;
}

/**
 * @brief addCar adds car into physical model
 * @param c is car instance
 */
void bullet::addCar(car* c) {

    /// Create car skin
    /**
     * experiment
    btConvexHullShape* compound = new btConvexHullShape();
    btTransform localTrans;
    localTrans.setIdentity();
    localTrans.setOrigin(btVector3(0, -c->wheelY / 2.0, 0));
    for (unsigned int i = 0; i < c->skin->models.size(); i++) {
        for (int j = 0; j < c->skin->models[i].triangleCount[0] * 3; j++) {
            compound->addPoint(btVector3(c->skin->models[i].vertices[j * 3 + 0],
                                         c->skin->models[i].vertices[j * 3 + 1],
                                         c->skin->models[i].vertices[j * 3 + 2]));
        }
    }*/

    /// set chassis
    btCollisionShape* chassisShape = new btBoxShape(btVector3(c->skin->width / 2.0f, c->skin->aplitude / 2.0f,c->skin->height / 2.0f));
    chassisShapes.push_back(chassisShape);
    btCompoundShape* compound = new btCompoundShape();
    compounds.push_back(compound);
    btTransform localTrans;
    localTrans.setIdentity();
    localTrans.setOrigin(btVector3(0, c->wheelY, 0));
    compound->addChildShape(localTrans,chassisShape);

    /// Set car physical values
    btVector3 localInertia(0,0,0);
    float mass = vehicleMassAspect * c->mass;
    compound->calculateLocalInertia(mass,localInertia);
    btRigidBody* m_carChassis = new btRigidBody(mass,0,compound,localInertia);
    bodies2.push_back(m_carChassis);
    m_dynamicsWorld->addRigidBody(m_carChassis);

    /// Set car default transform
    btVehicleRaycaster* m_vehicleRayCaster = new btDefaultVehicleRaycaster(m_dynamicsWorld);
    m_vehicleRayCasters.push_back(m_vehicleRayCaster);
    btTransform tr = *(new btTransform());
    tr.setIdentity();
    tr.setOrigin(btVector3(c->x,c->y - 0.1f,c->z));
    btQuaternion qn = *(new btQuaternion());
    qn.setW(btScalar(cos(c->rot * 3.14 / 180 / 2.0f)));
    qn.setX(btScalar(0));
    qn.setY(btScalar(sin(c->rot * 3.14 / 180 / 2.0f)));
    qn.setZ(btScalar(0));
    tr.setRotation(qn);

    /// Create car
    m_carChassis->setCenterOfMassTransform(tr);
    btRaycastVehicle::btVehicleTuning m_tuning = *(new btRaycastVehicle::btVehicleTuning());
    btRaycastVehicle *m_RaycastVehicle = new btRaycastVehicle(m_tuning,m_carChassis,m_vehicleRayCaster);
    m_RaycastVehicles.push_back(m_RaycastVehicle);
    m_vehicle.push_back(m_RaycastVehicle);
    m_carChassis->setActivationState(DISABLE_DEACTIVATION);
    //m_carChassis->setFriction(skinFriction);
    m_dynamicsWorld->addVehicle(m_vehicle[c->index - 1]);
    m_vehicle[c->index - 1]->setCoordinateSystem(0,1,2);

    /// Set wheels connections
    btVector3 wheelDirectionCS0(0,-1,0);
    btVector3 wheelAxleCS(-1,0,0);
    btScalar suspensionRestLength(0.5);
    btVector3 connectionPointCS0(-c->wheelX, 0, -c->wheelZ1);
    m_vehicle[c->index - 1]->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength,
                                      c->wheel->aplitude / 2.0f, m_tuning, true);
    connectionPointCS0 = btVector3(c->wheelX, 0, -c->wheelZ1);
    m_vehicle[c->index - 1]->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength,
                                      c->wheel->aplitude / 2.0f, m_tuning, true);
    connectionPointCS0 = btVector3(-c->wheelX, 0, c->wheelZ2);
    m_vehicle[c->index - 1]->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength,
                                      c->wheel->aplitude / 2.0f, m_tuning, false);
    connectionPointCS0 = btVector3(c->wheelX, 0, c->wheelZ2);
    m_vehicle[c->index - 1]->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength,
                                      c->wheel->aplitude / 2.0f, m_tuning, false);

    /// Set wheels parameters
    for (int i = 0; i < m_vehicle[c->index - 1]->getNumWheels(); i++) {
        btWheelInfo& wheel = m_vehicle[c->index - 1]->getWheelInfo(i);
        wheel.m_suspensionStiffness = suspensionStiffness;
        wheel.m_wheelsDampingRelaxation = suspensionDamping;
        wheel.m_wheelsDampingCompression = suspensionCompression;
        wheel.m_frictionSlip = wheelFriction;
        wheel.m_rollInfluence = rollInfluence;
    }
}

/**
 * @brief addModel adds model into physical model
 * @param m is 3D model for physical model
 */
void bullet::addModel(model *m) {
    for (unsigned int i = 0; i < m->models.size(); i++) {
        if (m->models[i].dynamic) {

            /// Create object
            float w = m->models[i].reg->max.x - m->models[i].reg->min.x;
            float a = m->models[i].reg->max.y - m->models[i].reg->min.y;
            float h = m->models[i].reg->max.z - m->models[i].reg->min.z;
            btCollisionShape* shape = new btBoxShape(btVector3(w / 2, a / 2, h / 2));
            shapes.push_back(shape);
            btCompoundShape* compound = new btCompoundShape();
            compounds.push_back(compound);
            btTransform localTrans;
            localTrans.setIdentity();
            compound->addChildShape(localTrans,shape);

            /// Set object physical values
            btVector3 localInertia(0,0,0);
            float mass = vehicleMassAspect * w * a * h;
            compound->calculateLocalInertia(mass,localInertia);
            btRigidBody* body = new btRigidBody(mass,0,compound,localInertia);
            bodies.push_back(body);
            m_dynamicsWorld->addRigidBody(body);

            /// Set object default transform
            btTransform tr = *(new btTransform());
            tr.setIdentity();
            tr.setOrigin(btVector3(m->models[i].reg->min.x+w/2, m->models[i].reg->min.y+a/2, m->models[i].reg->min.z+h/2));

            /// Create object
            body->setCenterOfMassTransform(tr);
            body->setFriction(skinFriction);
            m->models[i].dynamicID = bodies.size();

            /// set default position
            getTransform(m->models[i].dynamicID, mat);
            m->models[i].x = mat[12];
            m->models[i].y = mat[13];
            m->models[i].z = mat[14];
        } else if (m->models[i].touchable) {
            btTriangleMesh* mesh = new btTriangleMesh();
            meshes.push_back(mesh);
            for (int j = 0; j < m->models[i].triangleCount[m->cutX * m->cutY]; j++) {
                btVector3 a = btVector3(m->models[i].vertices[j * 9 + 0], m->models[i].vertices[j * 9 + 1], m->models[i].vertices[j * 9 + 2]);
                btVector3 b = btVector3(m->models[i].vertices[j * 9 + 3], m->models[i].vertices[j * 9 + 4], m->models[i].vertices[j * 9 + 5]);
                btVector3 c = btVector3(m->models[i].vertices[j * 9 + 6], m->models[i].vertices[j * 9 + 7], m->models[i].vertices[j * 9 + 8]);
                mesh->addTriangle(a, b, c);
            }
            btCollisionShape* levelShape = new btBvhTriangleMeshShape(mesh,true);
            levelShapes.push_back(levelShape);
            btVector3 localInertia(0,0,0);
            btTransform localTrans = *(new btTransform());
            localTrans.setIdentity();
            localTrans.setOrigin(btVector3(m->models[i].reg->min.x, m->models[i].reg->min.y, m->models[i].reg->min.z));
            btRigidBody* body = new btRigidBody(0,0,levelShape,localInertia);
            bodies2.push_back(body);
            body->setCenterOfMassTransform(localTrans);
            m_dynamicsWorld->addRigidBody(body);
        }
    }
}

/**
 * @brief getTransform counts OpenGL matrix of transformation
 * @param index is index of object
 * @return transformation matrix
 */
void bullet::getTransform(int index, float* m) {

    /// get matrix
    bodies[index]->getCenterOfMassTransform().getOpenGLMatrix(m);
    for (int i = 0; i < 16; i++) {
        if (isnan(m[i])) {
            for (int j = 0; j < 16; j++) {
                m[j] = eyemat[j];
            }
            return;
        }
    }
}

/**
 * @brief resetCar updates car state
 * @param c is instance of car
 */
void bullet::resetCar(car* c) {
    c->resetRequested = false;
    c->setStart(c->currentEdge, c->edgeSideMove);
    btTransform tr;
    tr.setIdentity();
    tr.setOrigin(btVector3(c->x,c->y - 0.1f,c->z));
    btQuaternion q;
    q.setRotation(btVector3(0,1,0), c->rot * 3.14 / 180.0);
    tr.setRotation(q);
    m_vehicle[c->index - 1]->getRigidBody()->setCenterOfMassTransform(tr);
}

/**
 * @brief updateCar updates car state
 * @param c is instance of car
 */
void bullet::updateCar(car* c) {

    /// get direction
    if ((int)m_vehicle[c->index - 1]->getCurrentSpeedKmHour() < 0)
        c->reverse = true;
    if ((int)m_vehicle[c->index - 1]->getCurrentSpeedKmHour() > 0)
        c->reverse = false;

    if (absf(m_vehicle[c->index - 1]->getCurrentSpeedKmHour()) < 5) {
        if (c->control->getGas() > 0)
            c->reverse = false;
        if (c->control->getBrake() > 0)
            c->reverse = true;
    }

    /// Set power
    float acc = c->acceleration;
    float max = (*c->gears)[c->currentGear].max;
    if (c->n2o < 150) {
        c->n2o += 0.1f;
    }
    if (c->control->getNitro()) {
        if (c->n2o >= 1) {
            acc *= 2;
            max *= 1.5f;
            c->n2o--;
        }
    }

    /// set forces
    float gEngineForce = c->control->getGas() * (engineMaxSpeed - c->speed) * acc * gasAspect;
    float gBreakingForce = c->control->getBrake() * c->brakePower * brakeAspect;
    if (c->reverse) {
        gEngineForce = c->control->getBrake() * (engineMaxSpeed - c->speed) * acc * gasAspect;
        gBreakingForce = c->control->getGas() * c->brakePower * brakeAspect / 2.0;
    }

    /// set auto braking
    if (c->speed < (*c->gears)[c->currentGear].min) {
        gEngineForce = 0;
        gBreakingForce = underSpeedBraking;
    }

    /// Limit power
    if (max < c->speed) {
        gEngineForce = 0;
        gBreakingForce = overSpeedBraking;
    }

    /// Apply power
    if (!locked) {
        float gVehicleSteering = c->control->getSteer() * c->steering;
            gVehicleSteering /= (steeringAspect + c->speed * steeringSpeedDependency);

        m_vehicle[c->index - 1]->setSteeringValue(gVehicleSteering,2);
        m_vehicle[c->index - 1]->setSteeringValue(gVehicleSteering,3);

        if (c->reverse) {
            m_vehicle[c->index - 1]->applyEngineForce(-gEngineForce,0);
            m_vehicle[c->index - 1]->applyEngineForce(-gEngineForce,1);
        } else {
            m_vehicle[c->index - 1]->applyEngineForce(gEngineForce,0);
            m_vehicle[c->index - 1]->applyEngineForce(gEngineForce,1);
        }
    }
    m_vehicle[c->index - 1]->setBrake(gBreakingForce + speedDecrease, 0);
    m_vehicle[c->index - 1]->setBrake(gBreakingForce + speedDecrease, 1);

    for (int i = 0; i < 4; i++)
        m_vehicle[c->index - 1]->updateWheelTransform(i,true);

    /// Other updates
    m_vehicle[c->index - 1]->updateVehicle(btScalar(vehicleStep));

    /// Reset car
    if ((c->speed < 5) && active && !physic->locked) {
        if (c->onRoof > 30) {
            c->resetAllowed = true;
            if (c->index - 1 != cameraCar) {
                c->resetRequested = true;
            }
            c->onRoof = 0;
        } else if (c->speed < 5) {
            c->onRoof++;
        }
    } else {
        c->resetAllowed = false;
    }

    /// check if it is possible to replace matrices
    if (!matrixLock) {

        /// count angle
        btQuaternion qn = m_vehicle[c->index - 1]->getRigidBody()->getOrientation();
        float rot = getRotation(qn.x(),qn.y(),qn.z(),qn.w());
        if (!isnan(rot)) {
            c->tempRot=rot;
        }

        /// get position
        float x = m_vehicle[c->index - 1]->getRigidBody()->getCenterOfMassPosition().getX();
        float y = m_vehicle[c->index - 1]->getRigidBody()->getCenterOfMassPosition().getY();
        float z = m_vehicle[c->index - 1]->getRigidBody()->getCenterOfMassPosition().getZ();
        if (!isnan(x) && !isnan(y) && !isnan(z)) {
            c->x=x;
            c->y=y;
            c->z=z;
        }

        /// get matrices
        for (int index = 0; index < 5; index++) {
            if (index > 0) {
              m_vehicle[c->index - 1]->getWheelInfo(index - 1).m_worldTransform.getOpenGLMatrix(c->transform[index].temp);
            } else {
              m_vehicle[c->index - 1]->getRigidBody()->getCenterOfMassTransform().getOpenGLMatrix(c->transform[index].temp);
            }
        }
    }

    // reset car
    if (c->resetAllowed && c->resetRequested) {
        for (int i = 0; i < carCount; i++) {
            if (i != c->index - 1) {
                if (dist(c->currentEdge.ax, c->currentEdge.ay, c->currentEdge.az, allCar[i]->x, allCar[i]->y, allCar[i]->z) < 10) {
                    return;
                }
            }
        }
        resetCar(c);
    }
}

/**
 * @brief updateWorld updates world state
 */
void bullet::updateWorld() {
     m_dynamicsWorld->stepSimulation(worldStep, worldSubStep);
}
