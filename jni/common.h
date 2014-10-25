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
#endif
#include <zip.h>
#include "interfaces/physics.h"
#include "car.h"

#define ALIASING 0.5

extern bool active;                    ///< Information if scene is active
extern std::vector<car*> allCar;       ///< All cars in scene instances
extern int cameraCar;                  ///< Car camera index
extern float direction;                ///< Camera direction
extern physics *physic;                ///< Physical engine instance
extern zip* APKArchive;                ///< Access to APK archive
extern int currentFrame;               ///< Frame index
extern float viewDistance;
#endif // COMMON_H
