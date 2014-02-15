//----------------------------------------------------------------------------------------
/**
 * \file       keyboard.cpp
 * \author     Vonasek Lubos
 * \date       2014/01/05
 * \brief      Car control device(or script) for navigating car on scene
*/
//----------------------------------------------------------------------------------------

#include "input/keyboard.h"
#include "common.h"

bool keys[7];            ///< State of keyboard

/// Key pressed
/**
 * @brief special Method is automatically called on key down
 * @param key is key code
 * @param x is mouse cursor position
 * @param y is mouse cursor position
 */
void special(int key, int x, int y) {
    lastkey = key;
    if (key == keyCameraDown)
        cameraDistance-=playerCameraStep;
    else if (key == keyCameraUp)
        cameraDistance+=playerCameraStep;
    else if (key == keyUp)
        keys[0] = true;
    else if (key == keyDown)
        keys[1] = true;
    else if (key == keyLeft)
        keys[2] = true;
    else if (key == keyRight)
        keys[3] = true;
    else if (key == keyGearUp)
        keys[4] = true;
    else if (key == keyGearDown)
        keys[5] = true;
    else if (key == 160)
        keys[6] = true;

    //printf("%d\n", key);

    if (cameraDistance < playerCameraMinimalDistance)
        cameraDistance = playerCameraMinimalDistance;
    if (cameraDistance > playerCameraMaximalDistance)
        cameraDistance = playerCameraMaximalDistance;
}

/// Key released
/**
 * @brief specialUp Method is automatically called on key up
 * @param key is key code
 * @param x is mouse cursor position
 * @param y is mouse cursor position
 */
void specialUp(int key, int x, int y) {

    if (key == keyUp)
        keys[0] = false;
    else if (key == keyDown)
        keys[1] = false;
    else if (key == keyLeft)
        keys[2] = false;
    else if (key == keyRight)
        keys[3] = false;
    else if (key == 160)
        keys[6] = false;
}

/**
 * @brief keyboard Creates new control instance
 */
keyboard::keyboard()
{
#ifndef ANDROID
    glutSpecialFunc(special);
    glutSpecialUpFunc(specialUp);
#endif
}

/**
 * @brief getBrake Get brakes state
 * @return value between 0 and 1 where 0=not braking and 1=braking
 */
float keyboard::getBrake() {
    if (keys[1])
        return 1;
    else
        return 0;
}

/**
 * @brief getDistance Get distance of camera from car for camera transformation
 * @return distance in float
 */
float keyboard::getDistance() {
    return cameraDistance;
}

/**
 * @brief getGas Get gas pedal state
 * @return value between 0 and 1 where 0=unpressed and 1=full pressed
 */
float keyboard::getGas() {
    if (keys[0] | keys[6])
        return 1;
    else
        return 0;
}

/**
 * @brief getGearChange Get gear change
 * @return 1 to gearUp, -1 to gearDown and 0 to stay
 */
int keyboard::getGearChange() {
    int output = 0;
    if (keys[4])
        output = -1;
    if (keys[5])
        output = 1;
    keys[4] = false;
    keys[5] = false;
    return output;
}

/**
 * @brief getNitro get nitro power
 * @return 0 if nitro is disable and 1 if it is enabled
 */
bool keyboard::getNitro() {
    return keys[6];
}

/**
 * @brief getSteer Get current volant state
 * @return value between -1 and 1 where -1=left, 0=center and 1=right
 */
float keyboard::getSteer() {
    if (keys[2] & keys[3])
        return 0;
    else if (keys[2])
        return 1;
    else if (keys[3])
        return -1;
    else
        return 0;
}

/**
 * @brief getUpdate Get minimal distance from car to next checkpoint, this value may be bigger
  for faster cars
 * @return constant distance in float
 */
float keyboard::getUpdate() {
    return playerTrackUpdate;
}
