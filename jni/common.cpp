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

/**
 * @brief The game resources
 */
std::vector<char*> carList;     ///< List of all cars
std::vector<shader*> shaders;   ///< Shaders storage
std::vector<texture*> textures; ///< Textures storage

#ifdef ANDROID
JNIEnv* instance;            ///< JNI instance
zip* APKArchive;             ///< APK archive instance
#endif

/**
 * The scene
 */
float aspect;                   ///< Screen aspect
pthread_mutex_t matrixLock;     ///< Car matrix locking
int screen_width;               ///< Screen width
int screen_height;              ///< Screen height
int playerCar;                  ///< Index of player car
bool active;                    ///< Information if scene is active
int opponentCount;              ///< Opponent count
model *skydome;                 ///< Skydome model
model *trackdata = 0;           ///< Track first model
model *trackdata2 = 0;          ///< Track second model
car *allCar[carLimit];          ///< All cars in scene instances
int carCount = 0;               ///< Amount of cars in scene
int cameraCar = 0;              ///< Car camera index
sound* crash;                   ///< Crash sound
sound* engine;                  ///< Engine sound
sound* enginePlus;              ///< Engine extra sound
sound* noise;                   ///< N2O sound
float *mat = new float[16];     ///< Temp matrix
char *string = new char[1024];  ///< Temp string
float direction = 0;            ///< Camera direction
float mtime;                    ///< Time stamp

physics *physic;     ///< Physical engine instance
renderer *xrenderer; ///< Renderer instance
#ifndef ANDROID
zip* APKArchive;     ///< Access to APK archive
#endif
