/----------------------------------------------------------------------------------------
/**
 * \file       aitraffic.h
 * \author     Vonasek Lubos
 * \date       2014/01/05
 * \brief      Car control device(or script) for navigating car on scene
*/
//----------------------------------------------------------------------------------------

#ifndef AITRAFFIC_H
#define AITRAFFIC_H

#include "interfaces/input.h"

/**
 * @brief The AI for traffic class
 */
class aitraffic : public input {
public:

    /**
     * @brief getBrake Get brakes state
     * @return value between 0 and 1 where 0=not braking and 1=braking
     */
    float getBrake();

    /**
     * @brief getDistance Get distance of camera from car for camera transformation
     * @return distance in float
     */
    float getDistance();

    /**
     * @brief getGas Get gas pedal state
     * @return value between 0 and 1 where 0=unpressed and 1=full pressed
     */
    float getGas();

    /**
     * @brief getGearChange Get gear change
     * @return 1 to gearUp, -1 to gearDown and 0 to stay
     */
    int getGearChange();

    /**
     * @brief getNitro get nitro power
     * @return 0 if nitro is disable and 1 if it is enabled
     */
    bool getNitro();

    /**
     * @brief getSteer Get current volant state
     * @return value between -1 and 1 where -1=left, 0=center and 1=right
     */
    float getSteer();

    /**
     * @brief getUpdate Get minimal distance from car to next checkpoint, this value may be bigger
      for faster cars
     * @return constant distance in float
     */
    float getUpdate();
};


#endif // AITRAFFIC_H
