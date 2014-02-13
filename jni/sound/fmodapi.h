//----------------------------------------------------------------------------------------
/**
 * \file       fmodapi.h
 * \author     Vonasek Lubos
 * \date       2014/01/05
 * \brief      Loading and playing sound
*/
//----------------------------------------------------------------------------------------

#ifndef FMODAPI_H
#define FMODAPI_H

#include "stdafx.h"
#include "fmod.hpp"

/**
 * @brief The fmodapi class is sound driver
 */
class fmodapi : public sound {
public:

    FMOD::Channel *channel[8];  ///< Channels for current sample
    FMOD::Sound   *sample;      ///< Current sample

    /**
     * @brief fmodapi create sound from file
     * @param filename is name of file to load
     * @param loop is true for sound looping
     * @param c is amount of channels
     */
    fmodapi(const char* filename, bool loop, int c);

    /**
     * @brief fmodapi destructor
     */
    virtual ~fmodapi();

    /**
     * @brief close kills sound system
     */
    void close();

    /**
     * @brief play starts playing sound
     * @param index is index of instance to play
     */
    void play(int index);

    /**
     * @brief setFrequency sets frequency of sound
     * @param index is index of instance to set
     * @param hz is frequency of sound
     */
    void setFrequency(int index, float hz);

    /**
     * @brief setPan sets pan of sound
     * @param index is index of instance to set
     * @param pan is stereo position
     */
    void setPan(int index, float pan);

    /**
     * @brief setVolume sets volume of sound
     * @param index is index of instance to set
     * @param volume is amount of volume
     */
    void setVolume(int index, float volume);

    /**
     * @brief stop stops playing sound
     * @param index is index of instance to stop
     */
    void stop(int index);
};

#endif // FMODAPI_H
