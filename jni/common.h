//----------------------------------------------------------------------------------------
/**
 * \file       common.h
 * \author     Vonasek Lubos
 * \date       2014/01/05
 * \brief      Common variables and objects in project
*/
//----------------------------------------------------------------------------------------

#ifndef COMMON_H
#define COMMON_H

#ifdef ANDROID
#define ZIP_ARCHIVE
#include <jni.h>
#include <zip.h>
extern JNIEnv* instance;            ///< JNI instance
extern zip* APKArchive;             ///< APK archive instance
#endif
#include <pthread.h>
#include <zip.h>
#include "interfaces/model.h"
#include "interfaces/physics.h"
#include "interfaces/renderer.h"
#include "interfaces/shader.h"
#include "interfaces/sound.h"
#include "interfaces/texture.h"
#include "car.h"

/**
 * @brief The game resources
 */
extern std::vector<char*>  carList;                 ///< List of all cars
extern std::vector<shader*> shaders;                ///< Shaders storage
extern std::vector<texture*> textures;              ///< Textures storage

/**
 * The scene
 */
extern float aspect;                                ///< Screen aspect
extern pthread_mutex_t matrixLock;                  ///< Car matrix locking
extern int playerCar;                               ///< Index of player car
extern bool active;                                 ///< Information if scene is active
extern int opponentCount;                           ///< Opponent count
extern model *skydome;                              ///< Skydome model
extern model *trackdata;                            ///< Track first model
extern model *trackdata2;                           ///< Track second model
const int carLimit = 8;                             ///< Maximum cars in game
extern car *allCar[carLimit];                       ///< All cars in scene instances
extern int carCount;                                ///< Amount of cars in scene
extern int cameraCar;                               ///< Car camera index
extern sound* crash;                                ///< Crash sound
extern sound* engine;                               ///< Engine sound
extern sound* enginePlus;                           ///< Engine extra sound
extern sound* noise;                                ///< N2O sound
extern float direction;                             ///< Camera direction

extern physics *physic;     ///< Physical engine instance
extern renderer *xrenderer; ///< Renderer instance
#ifndef ANDROID
extern zip* APKArchive;     ///< Access to APK archive
#endif

#endif // COMMON_H
