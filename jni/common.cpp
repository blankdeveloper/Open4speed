//----------------------------------------------------------------------------------------
/**
 * \file       common.cpp
 * \author     Vonasek Lubos
 * \date       2014/01/05
 * \brief      Common variables and objects in project
*/
//----------------------------------------------------------------------------------------

#include "common.h"

bool active;                    ///< Information if scene is active
float aliasing = 1.0;           ///< Screen detail
std::vector<car*> allCar;       ///< All cars in scene instances
int cameraCar = 0;              ///< Car camera index
float direction = 0;            ///< Camera direction
physics *physic;                ///< Physical engine instance
zip* APKArchive;                ///< Access to APK archive
int currentFrame = 0;           ///< Frame index
float viewDistance = 500;
