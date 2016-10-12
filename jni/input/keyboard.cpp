///----------------------------------------------------------------------------------------
/**
 * \file       keyboard.cpp
 * \author     Vonasek Lubos
 * \date       2014/12/30
 * \brief      Car control device(or script) for navigating car on scene
**/
///----------------------------------------------------------------------------------------

#ifndef ANDROID
#include <GL/freeglut.h>
#endif
#include "engine/switch.h"
#include "input/keyboard.h"
#include "renderers/opengl/gles20.h"

#define CAMERA_DISTANCE_DEFAULT 2.3
#define CAMERA_STEP 0.3
#define KEY_CAMERA_DOWN 171
#define KEY_CAMERA_UP 173
#define KEY_DOWN 103
#define KEY_LEFT 100
#define KEY_NITRO 160
#define KEY_RIGHT 102
#define KEY_UP 101
#define TRACK_UPDATE 80

float cameraDistance;    ///< Distance of camera
bool keys[5];            ///< State of keyboard

/// Key pressed
/**
 * @brief special Method is automatically called on key down
 * @param key is key code
 * @param x is mouse cursor position
 * @param y is mouse cursor position
 */
void special(int key, int x, int y) {
    if (key == KEY_CAMERA_DOWN)
        cameraDistance -= CAMERA_STEP;
    else if (key == KEY_CAMERA_UP)
        cameraDistance += CAMERA_STEP;
    else if (key == KEY_UP)
        keys[0] = true;
    else if (key == KEY_DOWN)
        keys[1] = true;
    else if (key == KEY_LEFT)
        keys[2] = true;
    else if (key == KEY_RIGHT)
        keys[3] = true;
    else if (key == KEY_NITRO)
        keys[4] = true;
}

/// Key released
/**
 * @brief specialUp Method is automatically called on key up
 * @param key is key code
 * @param x is mouse cursor position
 * @param y is mouse cursor position
 */
void specialUp(int key, int x, int y) {

    if (key == KEY_UP)
        keys[0] = false;
    else if (key == KEY_DOWN)
        keys[1] = false;
    else if (key == KEY_LEFT)
        keys[2] = false;
    else if (key == KEY_RIGHT)
        keys[3] = false;
    else if (key == KEY_NITRO)
        keys[4] = false;
}

/**
 * @brief keyboard Creates new control instance
 */
keyboard::keyboard()
{
    cameraDistance = CAMERA_DISTANCE_DEFAULT;
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
    if (keys[0] || keys[4])
        return 1;
    else
        return 0;
}

/**
 * @brief getNitro get nitro power
 * @return 0 if nitro is disable and 1 if it is enabled
 */
bool keyboard::getNitro() {
    return keys[4];
}

/**
 * @brief getSteer Get current volant state
 * @return value between -1 and 1 where -1=left, 0=center and 1=right
 */
float keyboard::getSteer() {
    if (keys[2] && keys[3])
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
    return TRACK_UPDATE;
}
