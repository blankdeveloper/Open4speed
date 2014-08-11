/----------------------------------------------------------------------------------------
/**
 * \file       aitraffic.cpp
 * \author     Vonasek Lubos
 * \date       2014/01/05
 * \brief      Car control device(or script) for navigating car on scene
*/
//----------------------------------------------------------------------------------------

#include "input/aitraffic.h"
#include "utils/math.h"
#include "common.h"

/**
 * @brief getBrake Get brakes state
 * @return value between 0 and 1 where 0=not braking and 1=braking
 */
float aitraffic::getBrake() {
    if (getGas() == 1)
        return 0;
    else
        return 1;
}

/**
 * @brief getDistance Get distance of camera from car for camera transformation
 * @return distance in float
 */
float aitraffic::getDistance() {
    return aiCameraDistance;
}

/**
 * @brief getGas Get gas pedal state
 * @return value between 0 and 1 where 0=unpressed and 1=full pressed
 */
float aitraffic::getGas() {

    /// prevent collision with other cars
    float safeDistance = aiTrafficSafeDistance + allCar[index]->speed / aiTrafficSafeDistanceSpeedDependency;
    for (int x = 0; x < carCount; x++) {
        if (((index != x) & (allCar[x] != 0)) & !allCar[x]->reverse)
            if (fabsf(allCar[x]->y - allCar[index]->y) < 1)
                if (distance(allCar[x], allCar[index], aiTrafficFutureStep) < safeDistance)
                    if ((int)(allCar[index]->rot - angle(allCar[x], allCar[index]) + 360) % 360 < aiTrafficSafeAngle)
                        return 0;
    }

    /// check angle of turn
    float g = gap(allCar[index]->currentEdge, allCar[index]);
    if (allCar[index]->speed > aiTrafficSafeSpeed) {
        if ((g > aiTrafficSafeGap) | (g < -aiTrafficSafeGap)) {
            return 0;
        }
    }
    if (allCar[index]->speed > aiTrafficSpeedBrake) {
           return 0;
    } else if (allCar[index]->speed > aiTrafficSpeedNoGas) {
        return 0.1;
    }
    return 1;
}

/**
 * @brief getGearChange Get gear change
 * @return 1 to gearUp, -1 to gearDown and 0 to stay
 */
int aitraffic::getGearChange() {
    return 0;
}

/**
 * @brief getNitro get nitro power
 * @return 0 if nitro is disable and 1 if it is enabled
 */
bool aitraffic::getNitro() {
    return false;
}

/**
 * @brief getSteer Get current volant state
 * @return value between -1 and 1 where -1=left, 0=center and 1=right
 */
float aitraffic::getSteer() {

    /// count track direction
    float g = gap(allCar[index]->currentEdge, allCar[index]);

    int w = 1;
    if (allCar[index]->reverse)
        w = -1;

    /// update direction
    if (g > aiTrafficSteeringGapLevel2)
        return -1 * w;
    if (g < -aiTrafficSteeringGapLevel2)
        return 1 * w;
    if (g > aiTrafficSteeringGapLevel1)
        return -0.5 * w;
    if (g < -aiTrafficSteeringGapLevel1)
        return 0.5 * w;
    return 0;
}

/**
 * @brief getUpdate Get minimal distance from car to next checkpoint, this value may be bigger
  for faster cars
 * @return constant distance in float
 */
float aitraffic::getUpdate() {    
    return aiTrafficTrackUpdate;
}
