//----------------------------------------------------------------------------------------
/**
 * \file       common.cpp
 * \author     Vonasek Lubos
 * \date       2014/01/05
 * \brief      Common variables and objects in project
*/
//----------------------------------------------------------------------------------------

#include "utils/io.h"
#include "common.h"

#ifdef ANDROID
JNIEnv* instance;            ///< JNI instance
#endif

/**
 * The scene
 */
float aspect;                   ///< Screen aspect
std::vector<std::string> carLst;///< List of all cars
pthread_mutex_t matrixLock;     ///< Car matrix locking
bool active;                    ///< Information if scene is active
int opponentCount;              ///< Opponent count
model *skydome;                 ///< Skydome model
model *trackdata = 0;           ///< Track first model
model *trackdata2 = 0;          ///< Track second model
std::vector<car*> allCar;       ///< All cars in scene instances
unsigned int cameraCar = 0;     ///< Car camera index
sound* crash;                   ///< Crash sound
sound* engine;                  ///< Engine sound
sound* enginePlus;              ///< Engine extra sound
sound* noise;                   ///< N2O sound
float direction = 0;            ///< Camera direction
physics *physic;                ///< Physical engine instance
renderer *xrenderer = 0;        ///< Renderer instance
zip* APKArchive;                ///< Access to APK archive
int currentFrame = 0;           ///< Frame index
Dynamic eff[effLen];            ///< 3D water effect object
model* water;                   ///< Water effect model
