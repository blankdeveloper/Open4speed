//----------------------------------------------------------------------------------------
/**
 * \file       fmodapi.cpp
 * \author     Vonasek Lubos
 * \date       2014/01/05
 * \brief      Loading and playing sound
*/
//----------------------------------------------------------------------------------------

#include "sound/fmodapi.h"
#include "utils/io.h"

FMOD::System *fmodsystem;          ///< Fmod sound system
bool fmodinitialized = false;      ///< Fmod initialization

/**
 * @brief fmodapi destructor
 */
fmodapi::~fmodapi() {
    delete sample;
    delete channel;
}

/**
 * @brief fmodapi create sound from file
 * @param filename is name of file to load
 * @param loop is true for sound looping
 * @param c is amount of channels
 */
fmodapi::fmodapi(const char* filename, bool loop, int channels) {

    /// create sound system
    if (!fmodinitialized) {
        FMOD::System_Create(&fmodsystem);
        fmodsystem->setOutput(FMOD_OUTPUTTYPE_AUTODETECT);
        fmodsystem->init(24, FMOD_INIT_NORMAL, 0);
        fmodinitialized = true;
    }
    if (loop)
        fmodsystem->createSound(prefix(filename), FMOD_SOFTWARE | FMOD_LOOP_NORMAL, 0, &sample);
    else
        fmodsystem->createSound(prefix(filename), FMOD_SOFTWARE, 0, &sample);
}

/**
 * @brief close kills sound system
 */
void fmodapi::close() {
    if (fmodinitialized) {
        fmodsystem->close();
        fmodinitialized = false;
    }
}

/**
 * @brief play starts playing sound
 * @param index is index of instance to play
 */
void fmodapi::play(int index) {
    bool playing;
    channel[index]->isPlaying(&playing);
    if (!playing)
        fmodsystem->playSound(FMOD_CHANNEL_FREE, sample, 0, &channel[index]);
}

/**
 * @brief setFrequency sets frequency of sound
 * @param index is index of instance to set
 * @param hz is frequency of sound
 */
void fmodapi::setFrequency(int index, float hz) {
    channel[index]->setFrequency(hz);
}

/**
 * @brief setPan sets pan of sound
 * @param index is index of instance to set
 * @param pan is stereo position
 */
void fmodapi::setPan(int index, float pan) {
    channel[index]->setPan(pan);
}

/**
 * @brief setVolume sets volume of sound
 * @param index is index of instance to set
 * @param volume is amount of volume
 */
void fmodapi::setVolume(int index, float volume) {
    channel[index]->setVolume(volume);
}

/**
 * @brief stop stops playing sound
 * @param index is index of instance to stop
 */
void fmodapi::stop(int index) {
    channel[index]->stop();
}
