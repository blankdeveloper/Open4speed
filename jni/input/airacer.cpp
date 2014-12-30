///----------------------------------------------------------------------------------------
/**
 * \file       airacer.cpp
 * \author     Vonasek Lubos
 * \date       2014/12/30
 * \brief      Car control device(or script) for navigating car on scene
**/
///----------------------------------------------------------------------------------------

#include "engine/math.h"
#include "engine/switch.h"
#include "input/airacer.h"

#define CAMERA_DISTANCE -2.0
#define REVERSE_DELAY 50
#define REVERSE_DURATION 30
#define SAFE_GAP_GAS 15
#define SAFE_SPEED 20
#define STEERING_GAP_LOW 5
#define STEERING_GAP_HIGH 15
#define TRACK_UPDATE 20
#define TRACK_UPDATE_SPEED_DEPENDENCY 20

/**
 * @brief airacer Creates new control instance
 */
airacer::airacer() {
    problem = 0;
    reverseMode = false;
}

/**
 * @brief getBrake Get brakes state
 * @ value between 0 and 1 where 0=not braking and 1=braking
 */
float airacer::getBrake() {
    if (getGas() > 0.8f)
         return 0;
    else
         return 0.5f;
}

/**
 * @brief getDistance Get distance of camera from car for camera transformation
 * @ distance in float
 */
float airacer::getDistance() {
     return CAMERA_DISTANCE;
}

/**
 * @brief getGas Get gas pedal state
 * @ value between 0 and 1 where 0=unpressed and 1=full pressed
 */
float airacer::getGas() {

    /// no gas on reverse mode
    if (reverseMode)
         return 0;

    /// check angle of turn
    if (getCar(index)->speed > SAFE_SPEED)
        if (fabsf(gap(getCar(index)->currentEdge.b, getCar(index)->pos, getCar(index)->rot)) > SAFE_GAP_GAS)
             return 0;
    if ((int)getCar(index)->speed > 5)
        problem = 0;
     return 1;
}

/**
 * @brief getNitro get nitro power
 * @return 0 if nitro is disable and 1 if it is enabled
 */
bool airacer::getNitro() {
     return false;
}

/**
 * @brief getSteer Get current volant state
 * @ value between -1 and 1 where -1=left, 0=center and 1=right
 */
float airacer::getSteer() {

    /// no turning on reverse mode
    if (reverseMode)
         return 0;

    /// count track direction
    float g2 = gap(getCar(index)->currentEdge.b, getCar(index)->pos, getCar(index)->rot);

    /// update direction
    if (g2 > STEERING_GAP_HIGH)
         return -1;
    if (g2 < -STEERING_GAP_HIGH)
         return 1;
    if (g2 > STEERING_GAP_LOW)
         return -0.5;
    if (g2 < -STEERING_GAP_LOW)
         return 0.5;
     return 0;
}

/**
 * @brief getUpdate Get minimal distance from car to next checkpoint, this value may be bigger
  for faster cars
 * @ constant distance in float
 */
float airacer::getUpdate() {
    if ((this->getGas() > 0) & ((int)getCar(index)->speed <= 5)) {
        problem++;
    }
    if (problem >= REVERSE_DELAY) {
        reverseMode = true;
        problem++;
    }
    if (problem >= REVERSE_DELAY + REVERSE_DURATION) {
        reverseMode = false;
        problem = 0;
    }
    return TRACK_UPDATE + getCar(index)->speed / TRACK_UPDATE_SPEED_DEPENDENCY;
}
