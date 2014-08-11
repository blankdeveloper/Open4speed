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
 * @brief renderLightmap render lightmaps and exit when it is true
 */
extern bool renderLightmap;

/**
 * @brief The game resources
 */
extern std::vector<char*> *carList;                                    ///< List of all cars
extern std::vector<char*> *trackList;                                  ///< List of all tracks
extern std::vector<shader*> *shaders;                                  ///< Shaders storage
extern std::vector<texture*> *textures;                                ///< Textures storage
extern texture *gray;                                                  ///< Gray color for meshes without material

/**
 * @brief The engine configs
 */
extern float aiCameraDistance;                                 ///< AI camera distance
extern float aiRacerFutureStep;                                ///< AI navigation parameter
extern float aiRacerFutureStepCount;                           ///< AI navigation parameter
extern float aiRacerReverseDelay;                              ///< AI navigation parameter
extern float aiRacerReverseDuration;                           ///< AI navigation parameter
extern float aiRacerSafeAngle;                                 ///< AI navigation parameter
extern float aiRacerSafeDistance;                              ///< AI navigation parameter
extern float aiRacerSafeDistanceSpeedDependency;               ///< AI navigation parameter
extern float aiRacerSafeGapGas;                                ///< AI navigation parameter
extern float aiRacerSafeGapSteering;                           ///< AI navigation parameter
extern float aiRacerSafeSpeed;                                 ///< AI navigation parameter
extern float aiRacerSteeringGapLevel1;                         ///< AI navigation parameter
extern float aiRacerSteeringGapLevel2;                         ///< AI navigation parameter
extern float aiRacerTrackUpdate;                               ///< AI navigation parameter
extern float aiRacerTrackUpdateSpeedDependency;                ///< AI navigation parameter
extern float aiTrafficFutureStep;                              ///< AI navigation parameter
extern float aiTrafficSafeAngle;                               ///< AI navigation parameter
extern float aiTrafficSafeDistance;                            ///< AI navigation parameter
extern float aiTrafficSafeDistanceSpeedDependency;             ///< AI navigation parameter
extern float aiTrafficSafeGap;                                 ///< AI navigation parameter
extern float aiTrafficSafeSpeed;                               ///< AI navigation parameter
extern float aiTrafficSpeedBrake;                              ///< AI navigation parameter
extern float aiTrafficSpeedNoGas;                              ///< AI navigation parameter
extern float aiTrafficSteeringGapLevel1;                       ///< AI navigation parameter
extern float aiTrafficSteeringGapLevel2;                       ///< AI navigation parameter
extern float aiTrafficTrackUpdate;                             ///< AI navigation parameter
extern char* gameMode;                                         ///< Screen mode
extern char* inputController;                                  ///< Input controller
extern float keyBack;                                          ///< Keycode
extern float keyCameraDown;                                    ///< Keycode
extern float keyCameraUp;                                      ///< Keycode
extern float keyDown;                                          ///< Keycode
extern float keyGearDown;                                      ///< Keycode
extern float keyGearUp;                                        ///< Keycode
extern float keyLeft;                                          ///< Keycode
extern float keyRight;                                         ///< Keycode
extern float keyUp;                                            ///< Keycode
extern float minimalPerspective;                               ///< Minimal perspective
extern float maximalPerspective;                               ///< Maximal perspective
extern float perspectiveSpeedDependency;                       ///< Perspective change speed dependency
extern float perspectiveSpeedFollow;                           ///< Perspective change speed
extern float playerCameraStep;                                 ///< Camera move step
extern float playerDefaultCameraDistance;                      ///< Camera distance
extern float playerTrackUpdate;                                ///< GPS navigation
extern char* physicalEngine;                                   ///< Used physical engine
extern char* screenRenderer;                                   ///< Used renderer
extern float soundCrashMinimalSpeed;                           ///< Crash sound minimal speed
extern float soundCrashOnSpeedChange;                          ///< Crash sound detection value
extern float soundEngineFreqAspect;                            ///< Engine frequency aspect
extern float soundMaximalDistance;                             ///< Maximal distance of sounds
extern float speedAspect;                                      ///< Speed aspect
extern float viewDistance;                                     ///< View distance

/**
 * The scene
 */
extern float aspect;                                ///< Menu aspect
extern pthread_mutex_t matrixLock;                  ///< Car matrix locking
extern int screen_width;                            ///< Screen width
extern int screen_height;                           ///< Screen height
extern float cameraDistance;                        ///< Camera distance level
extern int playerCar;                               ///< Index of player car
extern bool active;                                 ///< Information if scene is active
extern int currentTrack;                            ///< Current track index
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
extern float *mat;                                  ///< Temp matrix
extern char *string;                                ///< Temp string
const int configSize = 32;                          ///< Amount of config variables
extern int config[configSize];                      ///< Temporary config data
extern std::vector<char*> *configText[configSize];  ///< Config texts
extern float direction;                             ///< Camera direction
extern float mtime;                                 ///< Time stamp
extern int lastkey;                                 ///< Last pressed key keycode
extern bool renderLightmap;                         ///< Render lightmap mode enability
extern int rttsize;                                 ///< RTT texture size

extern physics *physic;     ///< Physical engine instance
extern renderer *xrenderer; ///< Renderer instance
#ifndef ANDROID
extern zip* APKArchive;     ///< Access to APK archive
#endif

/**
 * @brief loadAll load all variables from config file
 */
void loadAll();

#endif // COMMON_H
