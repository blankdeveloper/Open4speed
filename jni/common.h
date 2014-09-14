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
#include "interfaces/texture.h"
#include "car.h"

struct Dynamic {
    float* vertices;
    float* coords;
    int count;
    int frame;
};

const int effLen = 6;                  ///< Length of water effect

/**
 * The scene
 */
extern float aspect;                   ///< Screen aspect
extern std::vector<std::string> carLst;///< List of all cars
extern pthread_mutex_t matrixLock;     ///< Car matrix locking
extern bool active;                    ///< Information if scene is active
extern int opponentCount;              ///< Opponent count
extern model *skydome;                 ///< Skydome model
extern model *trackdata;               ///< Track first model
extern std::vector<car*> allCar;       ///< All cars in scene instances
extern unsigned int cameraCar;         ///< Car camera index
extern float direction;                ///< Camera direction
extern physics *physic;                ///< Physical engine instance
extern renderer *xrenderer;            ///< Renderer instance
extern zip* APKArchive;                ///< Access to APK archive
extern int currentFrame;               ///< Frame index
extern Dynamic eff[];                  ///< 3D water effect object
extern model* water;                   ///< Water effect model

#endif // COMMON_H
