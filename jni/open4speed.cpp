//----------------------------------------------------------------------------------------
/**
 * \file       open4speed.cpp
 * \author     Vonasek Lubos
 * \date       2014/01/05
 * \brief      Runable code of project.
*/
//----------------------------------------------------------------------------------------

#include "stdafx.h"

/**
 * Counters variables
 */
timespec ts_start, ts_end;          ///< Time messurement of rendering
float lastFrameTime = 0;            ///< Viewed time per frame
float frameTime = 0;                ///< Time per frame
int lastFPS = 0;                    ///< Viewed FPS
int fps = 0;                        ///< Actual FPS
int timestamp = 0;                  ///< Timestamp for updating FPS
GLuint id[2] = {0,1};               ///< GL Query ids

/**
 * @brief display updates display
 */
void display(void) {
    /// start messuring time
    clock_gettime(CLOCK_REALTIME, &ts_start);

    /// apply all scripts
    scriptLoop();

    /// update menu
    updateMenu();

#ifndef ANDROID
    /// start timer
    if (strcmp(screenRenderer, "glsl") == 0) {
        glGenQueries(2,id);
        glBeginQuery(GL_TIME_ELAPSED, id[0]);
        glBeginQuery(GL_SAMPLES_PASSED, id[1]);
    }
#endif

    /// display scene
    if (race) {

        displayScene();

        /// update FPS counter
        fps++;
        if (timestamp != time(0)) {
            lastFrameTime = frameTime / (float)fps;
            frameTime = 0;
            lastFPS = fps;
            timestamp = time(0);
            fps = 0;
            if (active) {
                if (!physic->locked) {
                    timeout--;
                    if (timeout == 0) {
                        std::vector<char*> *list = getList(timeoutAction);
                        syntaxList->assign(list->begin(), list->end());
                    }
                }
            }
        }
    }

    /// update sounds
    if (active) {
        for (int i = 0; i < carCount; i++) {
            allCar[i]->updateSound();
        }
    }

    /// display GUI
    displayMenu();

    /// draw FPS
    if (debug && race) {
        char text[100];
        sprintf(text, "FPS: %d, t: %0.3fms, e: %d, k: %d", lastFPS, lastFrameTime,
                allCar[cameraCar]->currentEdgeIndex, lastkey);
        xrenderer->renderText(0, 5, 1, text);
    }

#ifndef ANDROID
    /// stop timer
    GLint gpu_time = 0;
    GLint fragment_count = 0;
    if (strcmp(screenRenderer, "glsl") == 0) {
        glEndQuery(GL_SAMPLES_PASSED);
        glEndQuery(GL_TIME_ELAPSED);

        /// get scene counters
        glGetQueryObjectiv(id[0], GL_QUERY_RESULT, &gpu_time);
        glGetQueryObjectiv(id[1], GL_QUERY_RESULT, &fragment_count);
    }
    if (strcmp(screenRenderer, "glsl") == 0) {
        /// print counters
        char text[200];
        sprintf(text, "GPU time: %dk, fragments: %dk", gpu_time / 1000, fragment_count / 1000);
        xrenderer->renderText(0, 0, 1, text);
    }

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
    frameTime += (ts_end.tv_nsec - ts_start.tv_nsec) * 0.000001f;
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

    /// syntax called by escape key
    if ((syntaxList->size() == 0) & (key + 128 == (int)keyBack)) {
        std::vector<char*> *list = getList(esc);
        syntaxList->assign(list->begin(), list->end());
    }

    /* special hints for debugging
    if (key == 'a')
        physic->resetCar(allCar[cameraCar]);
    if (key == 's')
        testUniform--;
    if (key == 'd')
        testUniform++;
    if (key == 'q')
        cameraCar++;
    if (key == 'w')
        cameraCar--;
    if (cameraCar == -1)
        cameraCar = carCount - 1;
    if (cameraCar == carCount)
        cameraCar = 0;*/
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
 * @brief mouseClick is called on mouse click
 * @param button is index of pressed button
 * @param state is state of pressed button
 * @param x is mouse position x
 * @param y is mouse position y
 */
void mouseClick(int button, int state, int x, int y) {
    /// cancel if script is in progress
    if (syntaxList->size() > 0)
        return;

    /// set mouse button state
    if (button==0 && state==0)
        click = true;

    /// action on mouse button release
    if (button==0 && state==1 && click) {
        click = false;

        /// count position on screen
        x = x * 100 / (float)screen_width;
        y = y * 100 / (float)screen_height;

        /// check all buttons
        for (unsigned int i = 0; i < buttons->size(); i++) {

            /// check if button is clicked
            if ((x > (*buttons)[i].x) & (y > (*buttons)[i].y) & (x < (*buttons)[i].x + (*buttons)[i].width)
                & (y < (*buttons)[i].y + (*buttons)[i].height)) {
                exec = i;
                for (unsigned int j = 0; j < syntaxList->size(); j++) {
                    delete (*syntaxList)[j];
                }
                syntaxList = getList((*buttons)[i].syntax);
            }
        }
    }
}

/**
 * @brief mouseMove is called when mouse moves
 * @param x is mouse position x
 * @param y is mouse position y
 */
void mouseMove(int x, int y) {
    mouseX = x;
    mouseY = y;
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
            variable = 0;
            /// count player place
            for (int j = 0; j < carCount; j++) {
                if (allCar[cameraCar]->toFinish >= allCar[j]->toFinish) {
                    variable++;
                }
            }
            /// remove todolist
            for (unsigned int j = 0; j < syntaxList->size(); j++) {
                delete[] (*syntaxList)[j];
            }
            /// add finish action into todolist
            syntaxList = getList(finish);
        }
        /// another test if race is finished(solve finish line position problem)
        if (allCar[0]->lapsToGo == -1) {
            if (distance(allCar[cameraCar]->x, allCar[cameraCar]->z,
                         allCar[cameraCar]->currentGoalEdge.bx, allCar[cameraCar]->currentGoalEdge.bz) < 25) {

                /// remove todolist
                for (unsigned int j = 0; j < syntaxList->size(); j++) {
                    delete[] (*syntaxList)[j];
                }
                /// add finish action into todolist
                syntaxList = getList(finish);
            }
        }

        /// update cars navigation
        for (int i = 0; i < carCount; i++) {
            /// update current edge for navigation
            if (!physic->locked) {
                if ((distance(allCar[i]->currentEdge, allCar[i]) < allCar[i]->control->getUpdate())
                        && (absf(allCar[i]->currentEdge.by - allCar[i]->y) < 30)) {
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
   aspect = (GLfloat) w/(GLfloat) h;
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
    APKArchive = zip_open("../out/production/open4speedHD/open4speedHD.apk", 0, NULL);
#endif
#endif

    /// load game config
    if (exists(prefixEx("config"))) {
        FILE* file = fopen(prefixEx("config"), "r");
        for (int i = 0; i < configSize; i++) {
            fscanf(file,"%d\n", &config[i]);
        }
        fclose(file);
    } else {
        for (int i = 0; i < configSize; i++) {
            config[i] = 0;
        }
    }
    for (int i = 0; i < configSize; i++) {
        configText[i] = getList(getTag(i, "TEXTCFG%d"));
    }

    /// set menu variables
    background = -1;
    busy = false;
    click = false;
    debug = false;
    race = false;
    unlock = 0;

    /// load configuration
    loadAll();

#ifndef ANDROID
    /// init glut
    glutInit(&argc, argv);

    /// set screen mode
    if (strlen(gameMode) > 0) {
        glutGameModeString(gameMode);
        glutEnterGameMode();
    } else {
        glutInitWindowSize(960,640);
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
        glutCreateWindow("Open4speed");
        glutFullScreen();
    }

    /// set handlers
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);
    glutMouseFunc(mouseClick);
    glutPassiveMotionFunc(mouseMove);
#endif

    /// load menu data
    carList = getList("CARS");
    musicList = getList("MUSIC");
    syntaxList = getList("INIT");
    textList = getList("TEXTS");
    trackList = getList("TRACKS");

    /// start loop
#ifndef ANDROID
    glutTimerFunc(0,idle,0);
    glutMainLoop();
    return 0;
#endif
}

#ifdef ANDROID
JNIEnv* instance;            ///< JNI instance
zip* APKArchive;             ///< APK archive instance

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
 * @brief Java_com_lvonasek_o4s_O4SJNI_nativeClick is click method
 * @param env is instance of JNI
 * @param thiz is asset manager
 * @param x is position x
 * @param y is position y
 */
void Java_com_lvonasek_o4s_O4SJNI_nativeClick( JNIEnv*  env, jobject  thiz, jint x, jint y ) {
  mouseClick(0, 0, x, y);
  mouseClick(0, 1, x, y);
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
