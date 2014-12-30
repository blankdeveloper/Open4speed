///----------------------------------------------------------------------------------------
/**
 * \file       keyboard.h
 * \author     Vonasek Lubos
 * \date       2014/12/30
 * \brief      Car control device(or script) for navigating car on scene
**/
///----------------------------------------------------------------------------------------

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "interfaces/input.h"

/// Key pressed
/**
 * @brief special Method is automatically called on key down
 * @param key is key code
 * @param x is mouse cursor position
 * @param y is mouse cursor position
 */
extern void special(int key, int x, int y);


/// Key released
/**
 * @brief specialUp Method is automatically called on key up
 * @param key is key code
 * @param x is mouse cursor position
 * @param y is mouse cursor position
 */
extern void specialUp(int key, int x, int y);

/**
 * @brief The keyboard class
 */
class keyboard : public input {
public:

    /**
     * @brief keyboard Creates new control instance
     */
    keyboard();

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

#endif // KEYBOARD_H
