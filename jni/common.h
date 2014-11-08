//----------------------------------------------------------------------------------------
/**
 * \file       common.h
 * \author     Vonasek Lubos
 * \date       2014/11/08
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

extern bool active;                    ///< Information if scene is active
extern float aliasing;                 ///< Screen detail
extern std::vector<car*> allCar;       ///< All cars in scene instances
extern int cameraCar;                  ///< Car camera index
extern float direction;                ///< Camera direction
extern physics *physic;                ///< Physical engine instance
extern std::string shaderPath;         ///< Path to shader files
extern zip* APKArchive;                ///< Access to APK archive
extern int currentFrame;               ///< Frame index
extern float viewDistance;             ///< Maximal view distance
#endif // COMMON_H
