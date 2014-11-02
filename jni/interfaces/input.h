//----------------------------------------------------------------------------------------
/**
 * \file       input.h
 * \author     Vonasek Lubos
 * \date       2014/11/01
 * \brief      Car control device(or script) for navigating car on scene
*/
//----------------------------------------------------------------------------------------

#ifndef INPUT_H
#define INPUT_H

/**
 * @brief The input interface
 */
class input {
public:

    int index;          ///< Index of controlled car

    /**
     * @brief input destructor
     */
    virtual ~input() {}

    /**
     * @brief getBrake Get brakes state
     * @return value between 0 and 1 where 0=not braking and 1=braking
     */
    virtual float getBrake() = 0;

    /**
     * @brief getDistance Get distance of camera from car for camera transformation
     * @return distance in float
     */
    virtual float getDistance() = 0;

    /**
     * @brief getGas Get gas pedal state
     * @return value between 0 and 1 where 0=unpressed and 1=full pressed
     */
    virtual float getGas() = 0;

    /**
     * @brief getNitro get nitro power
     * @return 0 if nitro is disable and 1 if it is enabled
     */
    virtual bool getNitro() = 0;

    /**
     * @brief getSteer Get current volant state
     * @return value between -1 and 1 where -1=left, 0=center and 1=right
     */
    virtual float getSteer() = 0;

    /**
     * @brief getUpdate Get minimal distance from car to next checkpoint, this value may be bigger
      for faster cars
     * @return constant distance in float
     */
    virtual float getUpdate() = 0;
};

#endif // INPUT_H
