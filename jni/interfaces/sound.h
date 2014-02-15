//----------------------------------------------------------------------------------------
/**
 * \file       sound.h
 * \author     Vonasek Lubos
 * \date       2014/01/05
 * \brief      Loading and playing sound
*/
//----------------------------------------------------------------------------------------

#ifndef SOUND_H
#define SOUND_H


/**
 * @brief The sound interface
 */
class sound {
public:

    /**
     * @brief sound destructor
     */
    virtual ~sound() {}

    /**
     * @brief close kills sound system
     */
    virtual void close() = 0;

    /**
     * @brief play starts playing sound
     * @param index is index of instance to play
     */
    virtual void play(int index) = 0;

    /**
     * @brief setFrequency sets frequency of sound
     * @param index is index of instance to set
     * @param hz is frequency of sound
     */
    virtual void setFrequency(int index, float hz) = 0;

    /**
     * @brief setPan sets pan of sound
     * @param index is index of instance to set
     * @param pan is stereo position
     */
    virtual void setPan(int index, float pan) = 0;

    /**
     * @brief setVolume sets volume of sound
     * @param index is index of instance to set
     * @param volume is amount of volume
     */
    virtual void setVolume(int index, float volume) = 0;

    /**
     * @brief stop stops playing sound
     * @param index is index of instance to stop
     */
    virtual void stop(int index) = 0;
};

#endif // SOUND_H
