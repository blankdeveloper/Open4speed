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
#include <stdlib.h>
#include "input/airacer.h"
#include "input/keyboard.h"
#include "utils/io.h"
#include "utils/math.h"
#include "utils/switch.h"
#include "common.h"

#define VIEW_DISTANCE 500

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
 * @brief displayScene displaies race scene
 * @param renderer is instance of renderer
 * @param gui is instance of menu
 * @param track is instance of track
 * @param physics is instance of physical engine
 */
void displayScene() {

    /// update camera direction
    if (direction * 180 / 3.14 - allCar[cameraCar]->rot > 180)
        direction -= 6.28;
    else if (direction * 180 / 3.14 - allCar[cameraCar]->rot < -180)
        direction += 6.28;
    float g = allCar[cameraCar]->rot * 3.14 / 180.0 - direction;
    if (active) {
        if (allCar[cameraCar]->control->getDistance() < 0) {
            direction += g / 2.0;
        } else {
            direction += g / 15.0;
        }
    } else {
        direction += 0.01;
        if (direction > 6.28) {
            direction -= 6.28;
        }
    }

    /// fix camera direction
    int rot = allCar[cameraCar]->rot * 1000;
    rot += 720 * 1000;
    rot = rot % 360000;
    allCar[cameraCar]->rot = rot / 1000.0f;

    /// set camera
    xrenderer->rtt(true);
    float view = allCar[cameraCar]->getView();
    xrenderer->perspective(view, aspect, 0.5, VIEW_DISTANCE);
    xrenderer->pushMatrix();
    float cameraX = allCar[cameraCar]->transform->value[12] - sin(direction) * allCar[cameraCar]->control->getDistance() * 2 / (view / 90);
    float cameraY = allCar[cameraCar]->transform->value[13] + fabs(allCar[cameraCar]->control->getDistance() * 1.25f / (view / 90));
    float cameraZ = allCar[cameraCar]->transform->value[14] - cos(direction) * allCar[cameraCar]->control->getDistance() * 2 / (view / 90);
    xrenderer->lookAt(cameraX,cameraY,cameraZ,cameraX + sin(direction),cameraY + sin(-20 * 3.14 / 180), cameraZ + cos(direction),0,1,0);

    /// set light
    xrenderer->light.u_light_diffuse = glm::vec4(1000.0, 1000.0, 900.0, 0);
    xrenderer->light.u_light_cut = cos(120.0 * 3.14 / 180.0);
    xrenderer->light.u_light_spot_eff = 5.0;
    xrenderer->light.u_light_att = glm::vec4(5.0, 25.0, 125.0, 0);
    xrenderer->light.u_light_dir = glm::vec4(0.0, 0.75, 1.0, 0);
    xrenderer->pushMatrix();
    float* mat = allCar[cameraCar]->transform[0].value;
    glm::mat4x4 bulletMat = glm::mat4x4(mat[0], mat[1], mat[2], mat[3],
                                        mat[4], mat[5], mat[6], mat[7],
                                        mat[8], mat[9], mat[10], mat[11],
                                        mat[12], mat[13], mat[14], mat[15]);
    xrenderer->light.u_light = xrenderer->view_matrix * bulletMat * glm::vec4(0, allCar[cameraCar]->skin->aplitude * 0.5f,
                                                                              allCar[cameraCar]->skin->height * 0.6f, 1);
    xrenderer->light.u_light_dir = xrenderer->view_matrix * bulletMat * glm::vec4(0, allCar[cameraCar]->skin->aplitude * 3.0f,
                                                                                  -allCar[cameraCar]->skin->height, 0);
    xrenderer->popMatrix();

    /// render skydome
    xrenderer->pushMatrix();
    xrenderer->translate(cameraX, 0, cameraZ);
    xrenderer->scale(VIEW_DISTANCE * 0.9);
    xrenderer->renderModel(skydome);
    xrenderer->popMatrix();

    /// render track
    xrenderer->enable[1] = false;
    xrenderer->renderModel(trackdata);

    /// render cars
    xrenderer->enable[2] = false;
    for (int i = allCar.size() - 1; i >= 0; i--) {

        ///render car skin
        xrenderer->pushMatrix();
        xrenderer->multMatrix(allCar[i]->transform[0].value);
        xrenderer->enable[1] = allCar[i]->control->getNitro() && (allCar[i]->n2o > 1);
        xrenderer->enable[9] = allCar[i]->control->getBrake() != 0;
        xrenderer->renderModel(allCar[i]->skin);
        xrenderer->popMatrix();

        /// render wheels
        for (int j = 1; j <= 4; j++) {
            xrenderer->pushMatrix();
            xrenderer->multMatrix(allCar[i]->transform[j].value);
            if (j % 2 == 1)
              xrenderer->rotateY(180);
            xrenderer->renderModel(allCar[i]->wheel);
            xrenderer->popMatrix();
        }
    }

    /// render shadows
    xrenderer->shadowMode(true);
    for (int i = allCar.size() - 1; i >= 0; i--) {

        ///render car skin
        xrenderer->pushMatrix();
        xrenderer->multMatrix(allCar[i]->transform[0].value);
        xrenderer->renderShadow(allCar[i]->skin);
        xrenderer->popMatrix();

        /// render wheels
        for (int j = 1; j <= 4; j++) {
            xrenderer->pushMatrix();
            xrenderer->multMatrix(allCar[i]->transform[j].value);
            if (j % 2 == 1)
              xrenderer->rotateY(180);
            xrenderer->renderShadow(allCar[i]->wheel);
            xrenderer->popMatrix();
        }
    }
    xrenderer->shadowMode(false);

    /// render smoke effects
    for (int k = 0; k < effLen; k++) {
        if (active || (k != (currentFrame + effLen - 1) % effLen)) {
            water->models[0].texture2D->setFrame(eff[k].frame);
            xrenderer->renderDynamic(eff[k].vertices, eff[k].coords, water->models[0].material, water->models[0].texture2D, eff[k].count / 3);
        }
    }

    // update water
    eff[currentFrame].count = 0;
    for (int i = allCar.size() - 1; i >= 0; i--) {
        float effect = fabs(allCar[i]->speed) * fabs(allCar[i]->speed * (g + allCar[i]->control->getBrake() * 5.0)) * 0.001f;
        effect = fmax(effect, effect * 0.05 + allCar[i]->prevEffect * 0.95);
        allCar[i]->prevEffect = effect;
        for (int j = 1; j <= 4; j++) {
            if (active) {
                for (int k = 0; k < fmin(effect - 5, 5); k++) {
                    float x = allCar[i]->transform[j].value[12] + (rand() % 50 - 25) * 0.03f + sin(allCar[i]->rot * 3.14 / 180) * k * 0.03f;
                    float y = allCar[i]->transform[j].value[13] - 0.1f;
                    float z = allCar[i]->transform[j].value[14] + (rand() % 50 - 25) * 0.03f + cos(allCar[i]->rot * 3.14 / 180) * k * 0.03f;
                    for (int l = 0; l < water->models[0].triangleCount[1] * 3; l++) {
                        eff[currentFrame].vertices[eff[currentFrame].count * 3 + 0] = x;
                        eff[currentFrame].vertices[eff[currentFrame].count * 3 + 1] = y;
                        eff[currentFrame].vertices[eff[currentFrame].count * 3 + 2] = z;
                        eff[currentFrame].coords[eff[currentFrame].count * 2 + 0] = water->models[0].coords[l * 2 + 0];
                        eff[currentFrame].coords[eff[currentFrame].count * 2 + 1] = water->models[0].coords[l * 2 + 1];
                        eff[currentFrame].count++;
                    }
                }
            }
        }
    }
    xrenderer->popMatrix();

    // render RTT
    xrenderer->rtt(false);

    for (int k = 0; k < effLen; k++) {
        eff[k].frame++;
    }
    eff[currentFrame].frame = 0;
    currentFrame++;

    if (currentFrame >= effLen) {
        currentFrame = 0;
    }
}

/**
 * @brief loadScene loads scene from Open4speed config file
 * @param track is instance of last track
 * @param gui is instance of menu
 * @param atributes is track information
 * @param playerCar  is index of choosen car
 * @return loaded scene
 */
void loadScene(std::string filename) {

    /// load track
    std::vector<std::string> atributes = getList("", filename);
    trackdata = getModel(getConfigStr("track_model", atributes));

    /// load edges
#ifdef ZIP_ARCHIVE
    zip_file* file = zip_fopen(APKArchive, prefix(getConfigStr("track_edges", atributes)).c_str(), 0);
#else
    FILE* file = fopen(prefix(getConfigStr("track_edges", atributes)).c_str(), "r");
#endif
    char line[1024];
    gets(line, file);
    int edgesCount = scandec(line);
    int trackIndex = getConfig("race_track", atributes);
    std::vector<edge> e;
    for (int i = 0; i < edgesCount; i++) {
        gets(line, file);
        int edgeCount = scandec(line);
        for (int j = 0; j < edgeCount; j++) {
            edge value;
            gets(line, file);
            sscanf(line, "%f %f %f %f %f %f", &value.a.x, &value.a.y, &value.a.z, &value.b.x, &value.b.y, &value.b.z);
            if (i == trackIndex)
                e.push_back(value);
        }
        for (int j = 0; j < edgeCount; j++) {
            edge value;
            value.a.x = e[j].b.x;
            value.a.y = e[j].b.y;
            value.a.z = e[j].b.z;
            value.b.x = e[j].a.x;
            value.b.y = e[j].a.y;
            value.b.z = e[j].a.z;
            if (i == trackIndex)
                e.push_back(value);
        }
    }
#ifdef ZIP_ARCHIVE
    zip_fclose(file);
#else
    fclose(file);
#endif

    /// load sky
    skydome = getModel(getConfigStr("sky_model", atributes));

    /// load player car
    allCar.push_back(new car(getInput(), &e, carLst[0]));

    /// load race informations
    allCar[0]->lapsToGo = getConfig("laps", atributes) - 1;
    allCar[0]->finishEdge = getConfig("finish", atributes);
    allCar[0]->currentEdgeIndex = getConfig("player_start", atributes);
    allCar[0]->setStart(allCar[0]->edges[getConfig("player_start", atributes)]);

    /// load opponents
    opponentCount = getConfig("opponent_count", atributes);
    for (int i = 0; i < opponentCount; i++) {

        /// racer ai
        allCar.push_back(new car(new airacer(), &e, carLst[getConfig(getTag(i + 1, "opponent%d_car"), atributes)]));
        allCar[i + 1]->finishEdge = allCar[0]->finishEdge;
        allCar[i + 1]->lapsToGo = allCar[0]->lapsToGo;
        allCar[i + 1]->setStart(allCar[i + 1]->edges[getConfig(getTag(i + 1, "opponent%d_start"), atributes)]);
        allCar[i + 1]->currentEdgeIndex = getConfig(getTag(i + 1, "opponent%d_start"), atributes);
    }

    /// load water
    water = getModel("gfx/water.o4s");
    for (int i = 0; i < effLen; i++) {
        eff[i].vertices = new float[4095 * 3];
        eff[i].coords = new float[4095 * 2];
        eff[i].count = 0;
    }

    /// create instance of physical engine
    physic = getPhysics(trackdata);
    for (int i = 0; i <= opponentCount; i++)
        physic->addCar(allCar[i]);
    physic->locked = false;
}

/**
 * @brief idle is non-graphical thread and it is called automatically by GLUT
 * @param v is time information
 */
void idle(int v) {

    if (active) {
        /// update cars
        for (unsigned int i = 0; i < allCar.size(); i++)
            physic->updateCar(allCar[i]);
        /// update scene
        physic->updateWorld();

        /// update car transforms
        pthread_mutex_lock(&matrixLock);
        for (unsigned int i = 0; i < allCar.size(); i++)
            physic->updateCarTransform(allCar[i]);
        pthread_mutex_unlock(&matrixLock);

        /// if race finished show result
        if (allCar[0]->lapsToGo == -1) {
            int variable = 0;
            /// count player place
            for (unsigned int j = 0; j < allCar.size(); j++) {
                if (allCar[cameraCar]->toFinish >= allCar[j]->toFinish) {
                    variable++;
                }
            }

            //TODO what happend if player finish race
        }
        /// another test if race is finished(solve finish line position problem)
        if (allCar[0]->lapsToGo == -1) {
            if (distance(allCar[cameraCar]->pos, allCar[cameraCar]->currentGoalEdge.b) < 25) {

                //TODO what happend if player finish race
            }
        }

        /// update cars
        for (unsigned int i = 0; i < allCar.size(); i++) {
            /// update current edge for navigation
            if (!physic->locked) {
                if ((distance(allCar[i]->pos, allCar[i]->currentEdge.b) < allCar[i]->control->getUpdate())
                        && (fabsf(allCar[i]->currentEdge.b.y - allCar[i]->pos.y) < 30)) {
                    std::vector<int> nEdges = nextEdge(allCar[i]->edges, allCar[i]->currentEdge);
                    if (nEdges.size() > 0) {
                        allCar[i]->currentEdge = allCar[i]->edges[nEdges[0]];
                        printf("Car %d next edge: %d\n", i, nEdges[0]);
                    }

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

void *idle(void *threadid)
{
    idle(0);
    pthread_exit(NULL);
}

/**
 * @brief display updates display
 */
void display(void) {
    /// start messuring time
    clock_gettime(CLOCK_REALTIME, &ts_start);

    /// apply latest matrices
    pthread_mutex_lock(&matrixLock);
    for (int i = allCar.size() - 1; i >= 0; i--) {
        allCar[i]->updateMatrices();
    }
    pthread_mutex_unlock(&matrixLock);

    /// display scene
    displayScene();

#ifdef ANDROID
    idle(0);
    //pthread_t threads[1];
    //pthread_create(&threads[0], NULL, idle, (void *)0);
#endif

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
        printf("FPS: %d, t: %0.3fms, e: %d\n", lastFPS, lastFrameTime, allCar[cameraCar]->currentEdgeIndex);
#endif
    }

    /// update sounds
    if (active) {
        for (unsigned int i = 0; i < allCar.size(); i++)
          allCar[i]->updateSound();
    }

    /// stop messuring time
    clock_gettime(CLOCK_REALTIME, &ts_end);
    frameTime += (ts_end.tv_sec - ts_start.tv_sec + (ts_end.tv_nsec - ts_start.tv_nsec) * 0.000000001f) * 1000.0f;

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
#ifndef ANDROID
    if (key == 'q') {
        cameraCar--;
    }
    if (key == 'w') {
        cameraCar++;
    }
    if (cameraCar < 0)
        cameraCar = allCar.size() - 1;
    else if (cameraCar >= allCar.size())
        cameraCar = 0;
    if (key == 27) {
        active = false;
        delete physic;
        delete xrenderer;
        clearMediaStorage();
        while (!allCar.empty()) {
            delete allCar[allCar.size() - 1];
            allCar.pop_back();
        }
        exit(0);
    }
#endif
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
 * @brief reshape rescales window
 * @param w is new window width
 * @param h is new window hegiht
 */
void reshape (int w, int h) {
   aspect = (float) w/(float) h;
   if (xrenderer != 0)
       delete xrenderer;
   xrenderer = getRenderer(w, h);
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

#ifndef ANDROID
    /// init glut
    glutInit(&argc, argv);

    /// set screen mode
    glutInitWindowSize(960,640);
    glutInitContextVersion(3,0);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutCreateWindow("Open4speed");

    /// set handlers
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);
#endif

    /// load data
    carLst.push_back("cars/01/params");
    loadScene("tracks/track1");

    /// init sound
    crash = getSound("sfx/crash.ogg", false, 1);
    engine = getSound("sfx/engine.ogg", true, 1);
    enginePlus = getSound("sfx/engineplus.ogg", true, 1);
    noise = getSound("sfx/n2o.ogg", true, 1);

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
 * @brief Java_com_lvonasek_o4s_game_Native_init is init method
 * @param env is instance of JNI
 */
void Java_com_lvonasek_o4s_game_Native_init( JNIEnv*  env, jclass cls, jstring apkPath ) {
  instance = env;
  jboolean isCopy;
  APKArchive = zip_open(env->GetStringUTFChars(apkPath, &isCopy), 0, NULL);
  main(0, 0);
}

/**
 * @brief Java_com_lvonasek_o4s_game_Native_resize is resize method
 * @param env is instance of JNI
 * @param w is display width
 * @param h is display height
 */
void Java_com_lvonasek_o4s_game_Native_resize( JNIEnv*  env, jobject  thiz, jint w, jint h ) {
  reshape(w, h);
}

/**
 * @brief Java_com_lvonasek_o4s_game_Native_key is key press method
 * @param env is instance of JNI
 * @param thiz is asset manager
 * @param code is key code
 */
void Java_com_lvonasek_o4s_game_Native_key( JNIEnv*  env, jobject  thiz, jint code ) {
  special(code, 0, 0);
}

/**
 * @brief Java_com_lvonasek_o4s_game_Native_keyUp is key release method
 * @param env is instance of JNI
 * @param thiz is asset manager
 * @param code is key code
 */
void Java_com_lvonasek_o4s_game_Native_keyUp( JNIEnv*  env, jobject  thiz, jint code ) {
  specialUp(code, 0, 0);
}

/**
 * @brief Java_com_lvonasek_o4s_game_Native_display is display method
 * @param env is instance of JNI
 * @param thiz is asset manager
 */
void Java_com_lvonasek_o4s_game_Native_display( JNIEnv*  env, jobject  thiz ) {
  displayScene();
  glFinish();
}

/**
 * @brief Java_com_lvonasek_o4s_game_Native_loop is loop method
 * @param env is instance of JNI
 * @param thiz is asset manager
 */
void Java_com_lvonasek_o4s_game_Native_loop( JNIEnv*  env, jobject  thiz ) {
  display();
}

/**
 * @brief Java_com_lvonasek_o4s_game_Native_carCount is amount of cars
 * @param env is instance of JNI
 */
jint Java_com_lvonasek_o4s_game_Native_carCount( JNIEnv*  env ) {
  return allCar.size();
}

/**
 * @brief Java_com_lvonasek_o4s_game_Native_carState is information about car
 * @param env is instance of JNI
 */
jfloat Java_com_lvonasek_o4s_game_Native_carState( JNIEnv*  env, jobject  thiz, jint index, jint type ) {
    if (type == 0)
        return allCar[index]->speed;
    if (type == 1)
        return allCar[index]->control->getGas();
    if (type == 2)
        return allCar[index]->control->getBrake();
    if (type == 3)
        return allCar[index]->reverse ? 1 : -1;
    if (type == 4)
        return allCar[index]->lspeed;
    if (type == 5)
        return allCar[index]->gearLow;
    if (type == 6)
        return allCar[index]->gearHigh;
    if (type == 7)
        return allCar[index]->control->getNitro();
    if (type == 8)
        return allCar[index]->gears[allCar[index]->currentGear].min;
    if (type == 9)
        return allCar[index]->gears[allCar[index]->currentGear].max;
    if (type == 10)
        return allCar[index]->n2o;
    if (type == 11)
        return distance(allCar[cameraCar]->pos, allCar[index]->pos);
    return 0;
}
}
#endif
