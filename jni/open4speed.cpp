//----------------------------------------------------------------------------------------
/**
 * \file       open4speed.cpp
 * \author     Vonasek Lubos
 * \date       2014/01/05
 * \brief      Runable code of project.
*/
//----------------------------------------------------------------------------------------

#ifdef ANDROID
#include <GLES2/gl2.h>
#endif
#include "input/keyboard.h"
#include "utils/engine.h"
#include "utils/io.h"
#include "utils/math.h"
#include "utils/switch.h"
#include "common.h"

/**
 * Counters variables
 */
timespec ts_start, ts_end;          ///< Time messurement of rendering
float lastFrameTime = 0;            ///< Viewed time per frame
float frameTime = 0;                ///< Time per frame
int lastFPS = 0;                    ///< Viewed FPS
int fps = 0;                        ///< Actual FPS
int timestamp = 0;                  ///< Timestamp for updating FPS

/**
 * @brief display updates display
 */
void display(void) {
    /// start messuring time
    clock_gettime(CLOCK_REALTIME, &ts_start);

    /// display scene
    displayScene();

    /// update FPS counter
    fps++;
    if (timestamp != time(0)) {
        lastFrameTime = frameTime / (float)fps;
        frameTime = 0;
        lastFPS = fps;
        timestamp = time(0);
        fps = 0;

        /// draw FPS
#ifndef ANDROID
            printf("FPS: %d, t: %0.3fms, e: %d, k: %d\n", lastFPS, lastFrameTime, allCar[cameraCar]->currentEdgeIndex, lastkey);
#endif
    }

    /// update sounds
    if (active) {
        for (int i = 0; i < carCount; i++) {
            allCar[i]->updateSound();
        }
    }

#ifndef ANDROID
    /// check if there is an error
    int i = glGetError();
    if (i != 0) {
        printf("GL_ERROR %d\n", i);
    }

    /// finish rendering
    glutSwapBuffers();
#else
    glFinish();
#endif

    /// stop messuring time
    clock_gettime(CLOCK_REALTIME, &ts_end);
    frameTime += (ts_end.tv_sec - ts_start.tv_sec + (ts_end.tv_nsec - ts_start.tv_nsec) * 0.000000001f) * 1000.0f;
}

/**
 * @brief keyboard is called when key pressed
 * @param key is key code
 * @param x is cursor position x
 * @param y is cursor position y
 */
void keyboardDown(unsigned char key, int x, int y) {
    /// disable upper case
    if ((key >= 'A') & (key <= 'Z'))
        key = key - 'A' + 'a';
    /// resend key code
    special((int)key + 128, x, y);
}

/**
 * @brief keyboard is called when key released
 * @param key is key code
 * @param x is cursor position x
 * @param y is cursor position y
 */
void keyboardUp(unsigned char key, int x, int y) {
    /// disable upper case
    if ((key >= 'A') & (key <= 'Z'))
        key = key - 'A' + 'a';
    /// resend key code
    specialUp((int)key + 128, x, y);
}

/**
 * @brief idle is non-graphical thread and it is called automatically by GLUT
 * @param v is time information
 */
void idle(int v) {

    if (active) {
        /// update cars
        for (int i = 0; i < carCount; i++)
            physic->updateCar(allCar[i]);
        /// update scene
        physic->updateWorld();

        /// if race finished show result
        if (allCar[0]->lapsToGo == -1) {
            int variable = 0;
            /// count player place
            for (int j = 0; j < carCount; j++) {
                if (allCar[cameraCar]->toFinish >= allCar[j]->toFinish) {
                    variable++;
                }
            }

            //TODO what happend if player finish race
        }
        /// another test if race is finished(solve finish line position problem)
        if (allCar[0]->lapsToGo == -1) {
            if (distance(allCar[cameraCar]->x, allCar[cameraCar]->z,
                         allCar[cameraCar]->currentGoalEdge.bx, allCar[cameraCar]->currentGoalEdge.bz) < 25) {

                //TODO what happend if player finish race
            }
        }

        /// update cars navigation
        for (int i = 0; i < carCount; i++) {
            /// update current edge for navigation
            if (!physic->locked) {
                if ((distance(allCar[i]->currentEdge, allCar[i]) < allCar[i]->control->getUpdate())
                        && (fabsf(allCar[i]->currentEdge.by - allCar[i]->y) < 30)) {
                    std::vector<int> *nEdges = nextEdge(&allCar[i]->edges, allCar[i]->currentEdgeOriginal);
                    if (nEdges->size() > 0) {
                        allCar[i]->currentEdgeOriginal = allCar[i]->edges[(*nEdges)[0]];
                        allCar[i]->currentEdge = sidemoveEdge(allCar[i]->currentEdgeOriginal, allCar[i]->edgeSideMove);
                    }
                    delete nEdges;
                }
            }
            allCar[i]->update();
        }
    }

    /// call update
#ifndef ANDROID
    glutPostRedisplay();
    glutTimerFunc(50,idle,0);
#endif
}

/**
 * @brief reshape rescales window
 * @param w is new window width
 * @param h is new window hegiht
 */
void reshape (int w, int h) {
   aspect = (float) w/(float) h;
   screen_width = w;
   screen_height = h;
   xrenderer = getRenderer();
}

/**
 * @brief main loads data and prepares scene
 * @param argc is amount of arguments
 * @param argv is array of arguments
 * @return exit code
 */
int main(int argc, char** argv) {
#ifdef ZIP_ARCHIVE
#ifndef ANDROID
    /// open zip file with data
    APKArchive = zip_open("../out/production/Open4speed/Open4speed.apk", 0, NULL);
#endif
#endif

    /// set menu variables
    active = true;

    /// load configuration
    loadAll();

#ifndef ANDROID
    /// init glut
    glutInit(&argc, argv);

    /// set screen mode
    glutInitWindowSize(960,640);
    glutInitContextVersion(3,0);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
    glutCreateWindow("Open4speed");

    /// set handlers
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);
#endif

    /// load menu data
    carList = getList("CARS");
    trackList = getList("TRACKS");

    delete physic;
    currentTrack = 1;
#ifdef ZIP_ARCHIVE
    std::vector<char*> *atributes = getFullList(zip_fopen(APKArchive, prefix((*trackList)[currentTrack]), 0));
#else
    std::vector<char*> *atributes = getFullList(fopen(prefix((*trackList)[currentTrack]), "r"));
#endif
    loadScene(atributes);

    /// init sound
    crash = getSound("sfx/crash.ogg", false, 8);
    engine = getSound("sfx/engine.ogg", true, 8);
    noise = getSound("sfx/n2o.ogg", true, 8);

    /// create instance of physical engine
    physic = getPhysics(trackdata);
    if (trackdata2 != 0) {
        physic->addModel(trackdata2);
    }
    for (int i = 0; i <= opponentCount; i++)
        physic->addCar(allCar[i]);
    physic->locked = false;

    /// start loop
#ifndef ANDROID
    glutTimerFunc(0,idle,0);
    glutMainLoop();
    return 0;
#endif
}

#ifdef ANDROID
/**
 * Java native methods
 */
extern "C" {

/**
 * @brief Java_com_lvonasek_o4s_O4SJNI_nativeInit is init method
 * @param env is instance of JNI
 * @param am is asset manager
 */
void Java_com_lvonasek_o4s_O4SJNI_nativeInit( JNIEnv*  env, jclass cls, jstring apkPath ) {
  instance = env;
  jboolean isCopy;
  APKArchive = zip_open(env->GetStringUTFChars(apkPath, &isCopy), 0, NULL);
  main(0, 0);
}

/**
 * @brief Java_com_lvonasek_o4s_O4SJNI_nativeResize is resize method
 * @param env is instance of JNI
 * @param thiz is asset manager
 * @param w is display width
 * @param h is display height
 */
void Java_com_lvonasek_o4s_O4SJNI_nativeResize( JNIEnv*  env, jobject  thiz, jint w, jint h ) {
  reshape(w, h);
}

/**
 * @brief Java_com_lvonasek_o4s_O4SJNI_nativeKey is key press method
 * @param env is instance of JNI
 * @param thiz is asset manager
 * @param code is key code
 */
void Java_com_lvonasek_o4s_O4SJNI_nativeKey( JNIEnv*  env, jobject  thiz, jint code ) {
  special(code, 0, 0);
}

/**
 * @brief Java_com_lvonasek_o4s_O4SJNI_nativeKeyUp is key release method
 * @param env is instance of JNI
 * @param thiz is asset manager
 * @param code is key code
 */
void Java_com_lvonasek_o4s_O4SJNI_nativeKeyUp( JNIEnv*  env, jobject  thiz, jint code ) {
  specialUp(code, 0, 0);
}

/**
 * @brief Java_com_lvonasek_o4s_O4SJNI_nativeLoop is loop method
 * @param env is instance of JNI
 * @param thiz is asset manager
 */
void Java_com_lvonasek_o4s_O4SJNI_nativeDisplay( JNIEnv*  env, jobject  thiz ) {
  display();
}

/**
 * @brief Java_com_lvonasek_o4s_O4SJNI_nativeLoop is loop method
 * @param env is instance of JNI
 * @param thiz is asset manager
 */
void Java_com_lvonasek_o4s_O4SJNI_nativeLoop( JNIEnv*  env, jobject  thiz ) {
  idle(0);
}

/**
 * @brief Java_com_lvonasek_o4s_O4SJNI_nativeBack is back key method
 * @param env is instance of JNI
 * @param thiz is asset manager
 */
void Java_com_lvonasek_o4s_O4SJNI_nativeBack( JNIEnv*  env, jobject  thiz ) {
  keyboardDown(27, 0, 0);
}
}
#endif
