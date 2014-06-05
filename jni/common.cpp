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
  * Enabling and disabling lamps in time
  */
bool lamp[] = {0,0,0,0,1,0,1,1,0,1,
               1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,0,0,
               1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,1,1,1,1,1,
               1,1,1,1,1,0,0,1,0,1};

/**
 * @brief The game resources
 */
std::vector<char*> *carList;                                    ///< List of all cars
std::vector<texture*> *images = new std::vector<texture*>();    ///< List of all images
std::vector<char*> *textList;                                   ///< List of all texts
std::vector<char*> *trackList;                                  ///< List of all tracks
std::vector<char*> *syntaxList;                                 ///< List of syntaxes to do
std::vector<shader*> *shaders = new std::vector<shader*>();     ///< Shaders storage
std::vector<texture*> *textures = new std::vector<texture*>();  ///< Textures storage
texture *gray;                                                  ///< Gray color for meshes without material

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
float aiTrafficFutureStep;                              ///< AI navigation parameter
float aiTrafficSafeAngle;                               ///< AI navigation parameter
float aiTrafficSafeDistance;                            ///< AI navigation parameter
float aiTrafficSafeDistanceSpeedDependency;             ///< AI navigation parameter
float aiTrafficSafeGap;                                 ///< AI navigation parameter
float aiTrafficSafeSpeed;                               ///< AI navigation parameter
float aiTrafficSpeedBrake;                              ///< AI navigation parameter
float aiTrafficSpeedNoGas;                              ///< AI navigation parameter
float aiTrafficSteeringGapLevel1;                       ///< AI navigation parameter
float aiTrafficSteeringGapLevel2;                       ///< AI navigation parameter
float aiTrafficTrackUpdate;                             ///< AI navigation parameter
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
float playerCameraMaximalDistance;                      ///< Canera maximal distance
float playerCameraMinimalDistance;                      ///< Camera minimal distance
float playerCameraStep;                                 ///< Camera move step
float playerDefaultCameraDistance;                      ///< Camera distance
float playerTrackUpdate;                                ///< GPS navigation
char* physicalEngine;                                   ///< Used physical engine
char* screenRenderer;                                   ///< Used renderer
char* soundCrash;                                       ///< Crash sound file
float soundCrashMinimalSpeed;                           ///< Crash sound minimal speed
float soundCrashOnSpeedChange;                          ///< Crash sound detection value
char* soundEngine;                                      ///< Engine sound file
float soundEngineFreqAspect;                            ///< Engine frequency aspect
float soundMaximalDistance;                             ///< Maximal distance of sounds
float speedAspect;                                      ///< Speed aspect
float viewDistance;                                     ///< View distance

/**
 * The menu
 */
float aspect;                                                ///< Menu aspect
bool busy;                                                   ///< Busy information
bool click;                                                  ///< Click information
std::vector<button> *buttons = new std::vector<button>;      ///< List of buttons
bool debug;                                                  ///< Debug mode
char esc[32];                                                ///< Syntax to exec on esc
int exec;                                                    ///< Exec information
char finish[32];                                             ///< Syntax to exec on finish
bool race;                                                   ///< Race mode
int screen_width;                                            ///< Screen width
int screen_height;                                           ///< Screen height
int unlock;                                                  ///< Locking
int variable;                                                ///< Variable for scripts
bool updated;                                                ///< Information about updates

/**
 * The scene
 */

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
float soundVolume;              ///< Sound volume
sound* crash;                   ///< Crash sound
sound* engine;                  ///< Engine sound
sound* noise;                   ///< N2O sound
float *mat = new float[16];     ///< Temp matrix
char *string = new char[1024];  ///< Temp string
bool matrixLock = 0;            ///< Lock for replacing car matrices
bool renderLightmap = false;    ///< Render lightmap mode enability
int rttsize = 2048;             ///< RTT texture size

int config[configSize];                      ///< Temporary config data
std::vector<char*> *configText[configSize];  ///< Config texts
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
    std::vector<char*> *atributes = getList("CONFIG");
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
    aiTrafficFutureStep = getConfig("aiTrafficFutureStep", atributes);
    aiTrafficSafeAngle = getConfig("aiTrafficSafeAngle", atributes);
    aiTrafficSafeDistance = getConfig("aiTrafficSafeDistance", atributes);
    aiTrafficSafeDistanceSpeedDependency = getConfig("aiTrafficSafeDistanceSpeedDependency", atributes);
    aiTrafficSafeGap = getConfig("aiTrafficSafeGap", atributes);
    aiTrafficSafeSpeed = getConfig("aiTrafficSafeSpeed", atributes);
    aiTrafficSpeedBrake = getConfig("aiTrafficSpeedBrake", atributes);
    aiTrafficSpeedNoGas = getConfig("aiTrafficSpeedNoGas", atributes);
    aiTrafficSteeringGapLevel1 = getConfig("aiTrafficSteeringGapLevel1", atributes);
    aiTrafficSteeringGapLevel2 = getConfig("aiTrafficSteeringGapLevel2", atributes);
    aiTrafficTrackUpdate = getConfig("aiTrafficTrackUpdate", atributes);
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
    playerCameraMaximalDistance = getConfig("playerCameraMaximalDistance", atributes);
    playerCameraMinimalDistance = getConfig("playerCameraMinimalDistance", atributes);
    playerCameraStep = getConfig("playerCameraStep", atributes);
    playerDefaultCameraDistance = getConfig("playerDefaultCameraDistance", atributes);
    playerTrackUpdate = getConfig("playerTrackUpdate", atributes);
    physicalEngine = getConfigStr("physicalEngine", atributes);
    screenRenderer = getConfigStr("renderer", atributes);
    soundCrash = getConfigStr("soundCrash", atributes);
    soundCrashMinimalSpeed = getConfig("soundCrashMinimalSpeed", atributes);
    soundCrashOnSpeedChange = getConfig("soundCrashOnSpeedChange", atributes);
    soundEngine = getConfigStr("soundEngine", atributes);
    soundEngineFreqAspect = getConfig("soundEngineFreqAspect", atributes);
    soundMaximalDistance = getConfig("soundMaximalDistance", atributes);
    speedAspect = getConfig("speedAspect", atributes);
    viewDistance = getConfig("viewDistance", atributes);
    cameraDistance = playerDefaultCameraDistance;
    delete atributes;
}
