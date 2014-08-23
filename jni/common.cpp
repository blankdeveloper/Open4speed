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
std::vector<char*> *carList;                                    ///< List of all cars
std::vector<char*> *trackList;                                  ///< List of all tracks
std::vector<shader*> *shaders = new std::vector<shader*>();     ///< Shaders storage
std::vector<texture*> *textures = new std::vector<texture*>();  ///< Textures storage

#ifdef ANDROID
JNIEnv* instance;            ///< JNI instance
zip* APKArchive;             ///< APK archive instance
#endif

/**
 * @brief The engine configs
 */
float aiCameraDistance;                                 ///< AI camera distance
float aiRacerFutureStep;                                ///< AI navigation parameter
float aiRacerFutureStepCount;                           ///< AI navigation parameter
float aiRacerReverseDelay;                              ///< AI navigation parameter
float aiRacerReverseDuration;                           ///< AI navigation parameter
float aiRacerSafeAngle;                                 ///< AI navigation parameter
float aiRacerSafeDistance;                              ///< AI navigation parameter
float aiRacerSafeDistanceSpeedDependency;               ///< AI navigation parameter
float aiRacerSafeGapGas;                                ///< AI navigation parameter
float aiRacerSafeGapSteering;                           ///< AI navigation parameter
float aiRacerSafeSpeed;                                 ///< AI navigation parameter
float aiRacerSteeringGapLevel1;                         ///< AI navigation parameter
float aiRacerSteeringGapLevel2;                         ///< AI navigation parameter
float aiRacerTrackUpdate;                               ///< AI navigation parameter
float aiRacerTrackUpdateSpeedDependency;                ///< AI navigation parameter
char* inputController;                                  ///< Input controller
float keyBack;                                          ///< Keycode
float keyCameraDown;                                    ///< Keycode
float keyCameraUp;                                      ///< Keycode
float keyDown;                                          ///< Keycode
float keyGearDown;                                      ///< Keycode
float keyGearUp;                                        ///< Keycode
float keyLeft;                                          ///< Keycode
float keyRight;                                         ///< Keycode
float keyUp;                                            ///< Keycode
float minimalPerspective;                               ///< Minimal perspective
float maximalPerspective;                               ///< Maximal perspective
float perspectiveSpeedDependency;                       ///< Perspective change speed dependency
float perspectiveSpeedFollow;                           ///< Perspective change speed
float playerCameraStep;                                 ///< Camera move step
float playerDefaultCameraDistance;                      ///< Camera distance
float playerTrackUpdate;                                ///< GPS navigation
char* physicalEngine;                                   ///< Used physical engine
char* screenRenderer;                                   ///< Used renderer
float soundCrashMinimalSpeed;                           ///< Crash sound minimal speed
float soundCrashOnSpeedChange;                          ///< Crash sound detection value
float soundEngineFreqAspect;                            ///< Engine frequency aspect
float soundMaximalDistance;                             ///< Maximal distance of sounds
float speedAspect;                                      ///< Speed aspect
float viewDistance;                                     ///< View distance

/**
 * The scene
 */
float aspect;                   ///< Menu aspect
pthread_mutex_t matrixLock;     ///< Car matrix locking
int screen_width;               ///< Screen width
int screen_height;              ///< Screen height
float cameraDistance;           ///< Camera distance level
int playerCar;                  ///< Index of player car
bool active;                    ///< Information if scene is active
int currentTrack;               ///< Current track index
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
float direction = 0;                         ///< Camera direction
float mtime;                                 ///< Time stamp
int lastkey;                                 ///< Last pressed key keycode

physics *physic;     ///< Physical engine instance
renderer *xrenderer; ///< Renderer instance
#ifndef ANDROID
zip* APKArchive;     ///< Access to APK archive
#endif


/**
 * @brief loadAll load all variables from config file
 */
void loadAll() {
    std::vector<char*> *atributes = getList("CONFIG", "config/open4speed.txt");
    aiCameraDistance = getConfig("aiCameraDistance", atributes);
    aiRacerFutureStep = getConfig("aiRacerFutureStep", atributes);
    aiRacerFutureStepCount = getConfig("aiRacerFutureStepCount", atributes);
    aiRacerReverseDelay = getConfig("aiRacerReverseDelay", atributes);
    aiRacerReverseDuration = getConfig("aiRacerReverseDuration", atributes);
    aiRacerSafeAngle = getConfig("aiRacerSafeAngle", atributes);
    aiRacerSafeDistance = getConfig("aiRacerSafeDistance", atributes);
    aiRacerSafeDistanceSpeedDependency = getConfig("aiRacerSafeDistanceSpeedDependency", atributes);
    aiRacerSafeGapGas = getConfig("aiRacerSafeGapGas", atributes);
    aiRacerSafeGapSteering = getConfig("aiRacerSafeGapSteering", atributes);
    aiRacerSafeSpeed = getConfig("aiRacerSafeSpeed", atributes);
    aiRacerSteeringGapLevel1 = getConfig("aiRacerSteeringGapLevel1", atributes);
    aiRacerSteeringGapLevel2 = getConfig("aiRacerSteeringGapLevel2", atributes);
    aiRacerTrackUpdate = getConfig("aiRacerTrackUpdate", atributes);
    aiRacerTrackUpdateSpeedDependency = getConfig("aiRacerTrackUpdateSpeedDependency", atributes);
    inputController = getConfigStr("inputController", atributes);
    keyBack = getConfig("keyBack", atributes);
    keyCameraDown = getConfig("keyCameraDown", atributes);
    keyCameraUp = getConfig("keyCameraUp", atributes);
    keyDown = getConfig("keyDown", atributes);
    keyGearDown = getConfig("keyGearDown", atributes);
    keyGearUp = getConfig("keyGearUp", atributes);
    keyLeft = getConfig("keyLeft", atributes);
    keyRight = getConfig("keyRight", atributes);
    keyUp = getConfig("keyUp", atributes);
    maximalPerspective = getConfig("maximalPerspective", atributes);
    minimalPerspective = getConfig("minimalPerspective", atributes);
    perspectiveSpeedDependency = getConfig("perspectiveSpeedDependency", atributes);
    perspectiveSpeedFollow = getConfig("perspectiveSpeedFollow", atributes);
    playerCameraStep = getConfig("playerCameraStep", atributes);
    playerDefaultCameraDistance = getConfig("playerDefaultCameraDistance", atributes);
    playerTrackUpdate = getConfig("playerTrackUpdate", atributes);
    physicalEngine = getConfigStr("physicalEngine", atributes);
    screenRenderer = getConfigStr("renderer", atributes);
    soundCrashMinimalSpeed = getConfig("soundCrashMinimalSpeed", atributes);
    soundCrashOnSpeedChange = getConfig("soundCrashOnSpeedChange", atributes);
    soundEngineFreqAspect = getConfig("soundEngineFreqAspect", atributes);
    soundMaximalDistance = getConfig("soundMaximalDistance", atributes);
    speedAspect = getConfig("speedAspect", atributes);
    viewDistance = getConfig("viewDistance", atributes);
    cameraDistance = playerDefaultCameraDistance;
    delete atributes;
}
