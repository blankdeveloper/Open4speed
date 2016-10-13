///----------------------------------------------------------------------------------------
/**
 * \file       car.h
 * \author     Vonasek Lubos
 * \date       2014/12/30
 * \brief      Class for rendering car and updating navigation on track. It also loads
 *             informations about physical powers of current car.
**/
///----------------------------------------------------------------------------------------

#ifndef CAR_H
#define CAR_H

#include <string>
#include "interfaces/input.h"
#include "model.h"

/**
 * @brief The gear struct
 */
struct gear
{
    float min;  ///< Minimal value
    float max;  ///< Maximal value
};

/**
 * @brief The matrix struct
 */
struct matrix
{
    float* value;   ///< Used value
};

/**
 * @brief The car class
 */
class car {
public:
    float acceleration, brakePower, steering, power, lowAspect, mass;     ///< Physical parameters of car
    float wheelX, wheelY, wheelZ1, wheelZ2;                               ///< Wheel positions
    float gearUp, gearDown, gearLow, gearHigh, gearNitro;                 ///< Gear extremes
    std::vector<gear> gears;                                              ///< Transmision gears
    unsigned int currentGear;                                             ///< Current gear
    bool reverse;                                                         ///< Car direction
    input* control;                                                       ///< Car control device(or script)
    edge currentEdge;                                                     ///< Current track part
    int currentEdgeIndex;                                                 ///< Current position index
    edge currentGoalEdge;                                                 ///< Current track part for counting laps
    std::vector<edge> edges;                                              ///< Track for current car
    int finishEdge;                                                       ///< Index of final edge
    int lapsToGo;                                                         ///< Amount of laps to go
    unsigned int index;                                                   ///< Index of car
    glm::vec3 pos, oldPos;                                                ///< Car position
    float rot, speed, lspeed;                                             ///< Car state
    float view;                                                           ///< Camera view perspective angle
    model* skin;                                                          ///< 3D models
    model* wheel;                                                         ///< 3D models
    matrix* transform;                                                    ///< OpenGL matrix of transformation
    float toFinish;                                                       ///< Distance to finish
    int onRoof;                                                           ///< Time when car is on roof(used for reseting)
    bool resetAllowed, resetRequested;                                    ///< Reset variables
    float n2o;                                                            ///< Amount of N2O
    float extraSound;                                                     ///< Amount of extra engine sound volume
    float prevEffect;                                                     ///< Previous smoke effect intensity
    float sndCrash, sndDist, sndEngine1, sndEngine2, sndN2O, sndRate;     ///< Sound status


    /**
     * @brief car destructor
     */
    ~car();

    /**
     * @brief car is constructor which loads car model
     * @param i is car control device(or program)
     * @param e is curve which car may take on
     * @param filename is path to file to load
     * @param automatic is true for automatic transmision
     */
    car(input *i, std::vector<edge> *e, std::string filename);

    /**
     * @brief getView gets perspective view of car
     * @return view perspective by car speed
     */
    float getView();

    /**
     * @brief setStart sets start position of car
     * @param e is curve where car starts on
     */
    void setStart(edge e, float sidemove);

    /**
     * @brief update updates car wheels state(rotation and steering)
     */
    void update();
};

#endif // CAR_H
