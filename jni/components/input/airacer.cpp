//----------------------------------------------------------------------------------------
/**
 * \file       airacer.cpp
 * \author     Vonasek Lubos
 * \date       2014/01/05
 * \brief      Car control device(or script) for navigating car on scene
*/
//----------------------------------------------------------------------------------------

#include "stdafx.h"

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
    if (getGas() == 1)
         return 0;
    else
         return 1;
}

/**
 * @brief getDistance Get distance of camera from car for camera transformation
 * @ distance in float
 */
float airacer::getDistance() {
     return aiCameraDistance;
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
    if (allCar[index]->speed > aiRacerSafeSpeed)
        if (absf(gap(allCar[index]->currentEdge, allCar[index])) > aiRacerSafeGapGas)
             return 0;
    if ((int)allCar[index]->speed > 5)
        problem = 0;
     return 1;
}

/**
 * @brief getGearChange Get gear change
 * @ 1 to gearUp, -1 to gearDown and 0 to stay
 */
int airacer::getGearChange() {
     return 0;
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

    /// try to prevent collision with other cars
    /*float safeDistance = aiRacerSafeDistance + allCar[index]->speed / aiRacerSafeDistanceSpeedDependency;
    for (int x = 0; x < carCount; x++) {
        if (((index != x) & (allCar[x] != 0)) & !allCar[x]->reverse)
            if (absf(allCar[x]->y - allCar[index]->y) < 1)
                if (distance(allCar[x], allCar[index], aiRacerFutureStep) < safeDistance)
                    if ((int)(allCar[index]->rot - angle(allCar[x], allCar[index]) + 360) % 360 < aiRacerSafeAngle)
                        for (int i = 0; i < aiRacerFutureStepCount; i++) {
                            float g1 = gap(allCar[index], allCar[x], i);
                               if ((g1 > 0) & (g1 < aiRacerSafeGapSteering))
                                    return -1;
                               if ((g1 < 0) & (g1 > -aiRacerSafeGapSteering))
                                    return 1;
                        }
    }*/

    /// count track direction
    float g2 = gap(allCar[index]->currentEdge, allCar[index]);

    /// update direction
    if (g2 > aiRacerSteeringGapLevel2)
         return -1;
    if (g2 < -aiRacerSteeringGapLevel2)
         return 1;
    if (g2 > aiRacerSteeringGapLevel1)
         return -0.5;
    if (g2 < -aiRacerSteeringGapLevel1)
         return 0.5;
     return 0;
}

/**
 * @brief getUpdate Get minimal distance from car to next checkpoint, this value may be bigger
  for faster cars
 * @ constant distance in float
 */
float airacer::getUpdate() {

    /*if ((this->getGas() > 0) & ((int)allCar[index]->speed <= 5)) {
        problem++;
    }
    if (problem >= aiRacerReverseDelay) {
        reverseMode = true;
        problem++;
    }
    if (problem >= aiRacerReverseDelay + aiRacerReverseDuration) {
        reverseMode = false;
        problem = 0;
    }*/
     return aiRacerTrackUpdate + allCar[index]->speed / aiRacerTrackUpdateSpeedDependency;
}
