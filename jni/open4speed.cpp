///----------------------------------------------------------------------------------------
/**
 * \file       open4speed.cpp
 * \author     Vonasek Lubos
 * \date       2014/12/31
 * \brief      Runable code of project.
**/
///----------------------------------------------------------------------------------------

#ifdef ANDROID
#include <GLES2/gl2.h>
#include <jni.h>
#else
#include <GL/freeglut.h>
#endif
#include <stdlib.h>
#include "engine/io.h"
#include "engine/switch.h"
#include "input/airacer.h"
#include "input/keyboard.h"

struct Dynamic
{
    float* vertices;
    float* coords;
    int count;
    int frame;
};

float aspect;                   ///< Screen aspect
int cameraCar = 0;              ///< Car camera index
int currentFrame = 0;           ///< Frame index
float direction = 0;            ///< Camera direction
int viewDistance = 500;         ///< Camera view distance
const int effLen = 5;           ///< Length of water effect
model *skydome;                 ///< Skydome model
model *trackdata;               ///< Track first model
model *water;                   ///< Water effect model
Dynamic eff[effLen];            ///< 3D water effect object

/**
 * @brief display updates display
 */
void display(void)
{
    /// update camera direction
    if (direction * 180 / 3.14 - getCar(cameraCar)->rot > 180)
        direction -= 6.28;
    else if (direction * 180 / 3.14 - getCar(cameraCar)->rot < -180)
        direction += 6.28;
    float g = getCar(cameraCar)->rot * 3.14 / 180.0 - direction;
    if (getPhysics()->active)
    {
        if (getCar(cameraCar)->control->getDistance() < 0)
            direction += g / 2.0;
        else
            direction += g / 15.0;
    } else
    {
        direction += 0.01;
        if (direction > 6.28)
            direction -= 6.28;
    }

    /// fix camera direction
    int rot = getCar(cameraCar)->rot * 1000;
    rot += 720 * 1000;
    rot = rot % 360000;
    getCar(cameraCar)->rot = rot / 1000.0f;

    renderer* xrenderer = getRenderer();
    float view = getCar(cameraCar)->getView();
    xrenderer->rtt(true);
    /// set camera
    xrenderer->perspective(view, aspect, 0.5, viewDistance);
    xrenderer->pushMatrix();
    float cameraX = getCar(cameraCar)->transform->value[12];
    float cameraY = getCar(cameraCar)->transform->value[13];
    float cameraZ = getCar(cameraCar)->transform->value[14];
    //need for speed style
    cameraX -= sin(direction) * getCar(cameraCar)->control->getDistance() * 2.5f / (view / 90);
    cameraY += fmax(1.0, getCar(cameraCar)->control->getDistance()) / (view / 90);
    cameraZ -= cos(direction) * getCar(cameraCar)->control->getDistance() * 2.5f / (view / 90);
    xrenderer->lookAt(glm::vec3(cameraX - sin(direction) * 0.1f, cameraY + 0.5f, cameraZ - cos(direction) * 0.1f),
                      glm::vec3(cameraX + sin(direction) * 100.0f, cameraY, cameraZ + cos(direction) * 100.0f),
                      glm::vec3(0, 1, 0));

    /// render skydome
    xrenderer->pushMatrix();
    xrenderer->translate(cameraX, cameraY - 50, cameraZ);
    xrenderer->scale(viewDistance * 2 - 50);
    xrenderer->renderModel(skydome);
    xrenderer->popMatrix();

    /// render track
    xrenderer->enable[1] = false;
    xrenderer->renderModel(trackdata);

    /// render cars
    xrenderer->enable[2] = false;
    for (int i = getCarCount() - 1; i >= 0; i--)
    {
        ///render car skin
        xrenderer->pushMatrix();
        xrenderer->multMatrix(getCar(i)->transform[0].value);
        xrenderer->enable[1] = getCar(i)->control->getNitro() && (getCar(i)->n2o > 1);
        xrenderer->enable[9] = getCar(i)->control->getBrake() > 0.005f;
        xrenderer->renderModel(getCar(i)->skin);
        xrenderer->popMatrix();

        /// render wheels
        for (int j = 1; j <= 4; j++)
        {
            xrenderer->pushMatrix();
            xrenderer->multMatrix(getCar(i)->transform[j].value);
            if (j % 2 == 1)
              xrenderer->rotateY(180);
            xrenderer->renderModel(getCar(i)->wheel);
            xrenderer->popMatrix();
        }
    }

    /// render shadows
    for (int i = getCarCount() - 1; i >= 0; i--)
    {
        ///render car skin
        xrenderer->pushMatrix();
        xrenderer->multMatrix(getCar(i)->transform[0].value);
        xrenderer->renderShadow(getCar(i)->skin);
        xrenderer->popMatrix();
    }

    /// render smoke effects
    for (int k = 0; k < effLen; k++)
    {
        if (getPhysics()->active || (k != (currentFrame + effLen - 1) % effLen))
        {
            water->models[0].texture2D->setFrame(eff[k].frame);
            xrenderer->renderDynamic(eff[k].vertices, 0, eff[k].coords, water->models[0].material, water->models[0].texture2D, eff[k].count / 3);
        }
    }
    xrenderer->popMatrix();
    // render RTT
    xrenderer->rtt(false);

    // update water
    eff[currentFrame].count = 0;
    for (int i = getCarCount() - 1; i >= 0; i--)
    {
        float effect = fabs(getCar(i)->speed) * fabs(getCar(i)->speed * (g + getCar(i)->control->getBrake() * 5.0)) * 0.001f;
        effect = fmax(effect, effect * 0.05 + getCar(i)->prevEffect * 0.95);
        getCar(i)->prevEffect = effect;
        for (int j = 1; j <= 2; j++)
        {
            if (getPhysics()->active)
            {
                for (int k = 0; k < fmin(effect - 5, 5); k++)
                {
                    float x = getCar(i)->transform[j].value[12] + (rand() % 50 - 25) * 0.03f + sin(getCar(i)->rot * 3.14 / 180) * k * 0.03f;
                    float y = getCar(i)->transform[j].value[13] - 0.1f;
                    float z = getCar(i)->transform[j].value[14] + (rand() % 50 - 25) * 0.03f + cos(getCar(i)->rot * 3.14 / 180) * k * 0.03f;
                    for (int l = 0; l < water->models[0].vertices.size() / 3; l++)
                    {
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
    for (int k = 0; k < effLen; k++)
        eff[k].frame++;
    eff[currentFrame].frame = 0;
    currentFrame++;
    if (currentFrame >= effLen)
        currentFrame = 0;

#ifndef ANDROID
    /// check if there is an error
    int i = glGetError();
    if (i != 0)
        printf("GL_ERROR %d\n", i);

    /// finish rendering
    glutSwapBuffers();
#else
    glFinish();
#endif
}

/**
 * @brief loadScene loads scene from Open4speed config file
 * @param track is instance of last track
 * @param gui is instance of menu
 * @param atributes is track information
 * @param playerCar  is index of choosen car
 * @return loaded scene
 */
void loadScene(std::string filename)
{
    /// load track
    file* f = getFile(filename);
    std::string p = f->path();
    delete f;
    std::vector<std::string> atributes = getList("", filename);
    setShaderPath(p + getConfigStr("shaders", atributes));
    trackdata = getModel(p + getConfigStr("track_model", atributes));
    viewDistance = getConfig("view_distance", atributes);
    if (viewDistance == 0)
        viewDistance = 500;

    /// load edges
    std::vector<edge> e;
    if (getConfigStr("track_edges", atributes).length() > 0) {
        f = getFile(p + getConfigStr("track_edges", atributes));
        char line[1024];
        int edgesCount = f->scandec();
        int trackIndex = getConfig("race_track", atributes);
        for (int i = 0; i < edgesCount; i++) {
            int edgeCount = f->scandec();
            for (int j = 0; j < edgeCount; j++)
            {
                edge value;
                f->getsEx(line);
                sscanf(line, "%f %f %f %f %f %f", &value.a.x, &value.a.y, &value.a.z, &value.b.x, &value.b.y, &value.b.z);
                if (i == trackIndex)
                    e.push_back(value);
            }
            for (int j = 0; j < edgeCount; j++)
            {
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
        delete f;
    }
    else
    {
        edge edg;
        edg.a = glm::vec3( 100, 50, 0 );
        edg.b = glm::vec3( 10, 50, 0 );
        e.push_back( edg );
    }

    /// load sky
    skydome = getModel(p + getConfigStr("sky_model", atributes));

    /// load player car
    addCar(new car(getInput(), &e, p + getConfigStr("player_car", atributes)));

    /// load race informations
    getCar(0)->lapsToGo = getConfig("laps", atributes);
    getCar(0)->finishEdge = getConfig("finish", atributes);
    getCar(0)->currentEdgeIndex = getConfig("race_start", atributes);
    if (!getCar(0)->edges.empty())
    {
      getCar(0)->setStart(getCar(0)->edges[getConfig("race_start", atributes)], 0);

      /// load opponents
      int opponentCount = getConfig("opponent_count", atributes);
      for (int i = 0; i < opponentCount; i++)
      {
          /// racer ai
          airacer* ai = new airacer();
          addCar(new car(ai, &e, p + getConfigStr("opponent" + str(i + 1) + "_car", atributes)));
          ai->init(getCar(i + 1));
          getCar(i + 1)->finishEdge = getCar(0)->finishEdge;
          getCar(i + 1)->lapsToGo = getCar(0)->lapsToGo;
          int move = (i % 2) * 2 - 1;
          getCar(i + 1)->setStart(getCar(i + 1)->edges[getConfig("race_start", atributes)], move * 4);
          getCar(i + 1)->currentEdgeIndex = getConfig("race_start", atributes);
      }
    }

    /// load water
    water = getModel("#assets/cars/fx/water.o4s");
    for (int i = 0; i < effLen; i++)
    {
        eff[i].vertices = new float[4095 * 3];
        eff[i].coords = new float[4095 * 2];
        eff[i].count = 0;
    }

    /// create instance of physical engine
    physics* physic = getPhysics();
    physic->addModel(trackdata);
    for (unsigned int i = 0; i < getCarCount(); i++)
        physic->addCar(getCar(i));

    /// optimise level
    trackdata->culling();
}

/**
 * @brief idle is non-graphical thread and it is called automatically by GLUT
 * @param v is time information
 */
void idle(int v)
{
    physics* physic = getPhysics();
    if (physic->active)
    {
        /// update cars
        for (unsigned int i = 0; i < getCarCount(); i++)
        {
            /// update current edge for navigation
            car* c = getCar(i);
            if (!physic->locked)
            {
                bool out = distance(c->pos, getCar(0)->pos) > 300;
                float update = out ? 5 : c->control->getUpdate();
                if ((distance(c->pos, c->currentEdge.b) < update)
                        && (fabsf(c->currentEdge.b.y - c->pos.y) < 30))
                {
                    std::vector<int> nEdges = nextEdge(c->edges, c->currentEdge);
                    if (nEdges.size() > 0)
                        c->currentEdge = c->edges[nEdges[0]];

                }
                if (i != 0)
                {
                    if (out)
                    {
                      int rnd = (int)(i * 17) % 10;
                      float speed = (1.5f + rnd * 0.1f) * (1 - c->control->getBrake());
                      c->pos += glm::normalize(c->currentEdge.b - c->pos) * speed;
                      c->rot = angle(c->currentEdge.b, c->currentEdge.a) * 180.0f / 3.14f;
                      physic->resetCar(c, false);
                      continue;
                    }
                }
            }
            physic->updateCar(c);
            c->update();

            // reset car if needed
            if (c->resetAllowed && c->resetRequested)
            {
                for (unsigned int i = 0; i < getCarCount(); i++)
                {
                    if (i != c->index - 1)
                        if (glm::length(c->currentEdge.a - getCar(i)->pos) < 10)
                            return;
                }
                physic->resetCar(c);
            }
        }

        /// update dynamic objects
        for (unsigned int i = 0; i < trackdata->models.size(); i++)
            if (trackdata->models[i].dynamic)
                getPhysics()->getTransform(trackdata->models[i].dynamicID, trackdata->models[i].dynamicMat);

        /// update scene
        physic->updateWorld();
    }

    /// call update
#ifndef ANDROID
    glutPostRedisplay();
    glutTimerFunc(50,idle,0);
#endif
}

/**
 * @brief keyboard is called when key pressed
 * @param key is key code
 * @param x is cursor position x
 * @param y is cursor position y
 */
void keyboardDown(unsigned char key, int x, int y)
{
    /// disable upper case
    if ((key >= 'A') & (key <= 'Z'))
        key = key - 'A' + 'a';
    /// resend key code
    keyboard::special((int)key + 128, x, y);
#ifndef ANDROID
    if (key == 'q')
        cameraCar--;
    else if (key == 'w')
        cameraCar++;
    else if (key == 'r')
        getPhysics()->resetCar(getCar(0), false);

    if (cameraCar < 0)
        cameraCar = getCarCount() - 1;
    else if (cameraCar >= (int)getCarCount())
        cameraCar = 0;
#endif
}

void unload()
{
    getPhysics()->active = false;
    clearMediaStorage();
}

/**
 * @brief keyboard is called when key released
 * @param key is key code
 * @param x is cursor position x
 * @param y is cursor position y
 */
void keyboardUp(unsigned char key, int x, int y)
{
    /// disable upper case
    if ((key >= 'A') & (key <= 'Z'))
        key = key - 'A' + 'a';
    /// resend key code
    keyboard::specialUp((int)key + 128, x, y);
}

/**
 * @brief reshape rescales window
 * @param w is new window width
 * @param h is new window hegiht
 */
void reshape(int w, int h)
{
   aspect = (float) w/(float) h;
   getRenderer()->init(w, h);
}

#ifdef ANDROID
/**
 * Java native methods
 */
extern "C"
{

/**
 * @brief Java_com_lvonasek_o4s_game_GameLoop_init is init method
 * @param env is instance of JNI
 */
void Java_com_lvonasek_o4s_game_GameLoop_init( JNIEnv* env, jclass cls, jstring apkPath, jstring track, float alias )
{
  jboolean isCopy;
  setZip(env->GetStringUTFChars(apkPath, &isCopy));
  getPhysics()->active = true;
  getRenderer()->aliasing = alias;

  //prevent start without FBO
  std::string filename = env->GetStringUTFChars(track, &isCopy);
  file* f = getFile(filename);
  std::string p = f->path();
  delete f;
  setShaderPath(p + getConfigStr("shaders", getList("", filename)));
  reshape(64, 64);

  loadScene(filename);
}

/**
 * @brief Java_com_lvonasek_o4s_game_GameLoop_resize is resize method
 * @param env is instance of JNI
 * @param w is display width
 * @param h is display height
 */
void Java_com_lvonasek_o4s_game_GameLoop_resize( JNIEnv* env, jobject object, jint w, jint h )
{
  reshape(w, h);
}
/**
 * @brief Java_com_lvonasek_o4s_game_GameLoop_restart restarts player car
 * @param env is instance of JNI
 */
void Java_com_lvonasek_o4s_game_GameLoop_restart( JNIEnv* env, jobject object)
{
  getPhysics()->resetCar(getCar(0));
}

/**
 * @brief Java_com_lvonasek_o4s_game_GameLoop_key is key press method
 * @param env is instance of JNI
 * @param object is asset manager
 * @param code is key code
 */
void Java_com_lvonasek_o4s_game_GameLoop_key( JNIEnv* env, jobject object, jint code )
{
  special(code, 0, 0);
}

/**
 * @brief Java_com_lvonasek_o4s_game_GameLoop_keyUp is key release method
 * @param env is instance of JNI
 * @param object is asset manager
 * @param code is key code
 */
void Java_com_lvonasek_o4s_game_GameLoop_keyUp( JNIEnv* env, jobject object, jint code )
{
  specialUp(code, 0, 0);
}

/**
 * @brief Java_com_lvonasek_o4s_game_GameLoop_display is display method
 * @param env is instance of JNI
 * @param object is asset manager
 */
void Java_com_lvonasek_o4s_game_GameLoop_display( JNIEnv* env, jobject object )
{
  display();
}

/**
 * @brief Java_com_lvonasek_o4s_game_GameLoop_loop is loop method
 * @param env is instance of JNI
 * @param object is asset manager
 */
void Java_com_lvonasek_o4s_game_GameLoop_loop( JNIEnv* env, jobject object )
{
  idle(0);
}

/**
 * @brief Java_com_lvonasek_o4s_game_GameLoop_unload unloads resources
 * @param env is instance of JNI
 * @param object is asset manager
 */
void Java_com_lvonasek_o4s_game_GameLoop_unload( JNIEnv* env, jobject object )
{
  unload();
}

/**
 * @brief Java_com_lvonasek_o4s_game_GameLoop_unlock unlocks physics
 * @param env is instance of JNI
 * @param object is asset manager
 */
void Java_com_lvonasek_o4s_game_GameLoop_unlock( JNIEnv* env, jobject object )
{
    getPhysics()->locked = false;
}

/**
 * @brief Java_com_lvonasek_o4s_game_GameLoop_carCount is amount of cars
 * @param env is instance of JNI
 */
jint Java_com_lvonasek_o4s_game_GameLoop_carCount( JNIEnv* env )
{
  return getCarCount();
}

/**
 * @brief Java_com_lvonasek_o4s_game_GameLoop_carState is information about car
 * @param env is instance of JNI
 */
jfloat Java_com_lvonasek_o4s_game_GameLoop_carState( JNIEnv* env, jobject object, jint index, jint type )
{
    if (type == 0)
        return getCar(index)->speed;
    if (type == 1)
        return getCar(index)->control->getGas();
    if (type == 2)
        return getCar(index)->control->getBrake();
    if (type == 3)
        return getCar(index)->reverse ? 1 : -1;
    if (type == 4)
        return getCar(index)->lspeed;
    if (type == 5)
        return getCar(index)->gearLow;
    if (type == 6)
        return getCar(index)->gearHigh;
    if (type == 7)
        return getCar(index)->control->getNitro();
    if (type == 8)
        return getCar(index)->gears[getCar(index)->currentGear].min;
    if (type == 9)
        return getCar(index)->gears[getCar(index)->currentGear].max;
    if (type == 10)
        return getCar(index)->n2o;
    if (type == 11)
        return distance(getCar(cameraCar)->pos, getCar(index)->pos);
    if (type == 12) {
        bool output = getCar(index)->sndCrash;
        getCar(index)->sndCrash = false;
        return output;
    }
    if (type == 13)
        return getCar(index)->sndDist;
    if (type == 14)
        return getCar(index)->sndEngine1;
    if (type == 15)
        return getCar(index)->sndEngine2;
    if (type == 16)
        return getCar(index)->sndN2O;
    if (type == 17)
        return getCar(index)->sndRate;
    if (type == 18)
        return getCar(index)->edges.empty() ? 9999 : getCar(index)->toFinish;
    return 0;
}
}
#else

/**
 * @brief main loads data and prepares scene
 * @param argc is amount of arguments
 * @param argv is array of arguments
 * @return exit code
 */
int main(int argc, char** argv)
{
    /// init glut
    glutInit(&argc, argv);
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

    /// load data
    loadScene("#assets/tracks/winter-day.o4scfg");

    /// start loop
    getPhysics()->locked = false;
    atexit(unload);
    glutTimerFunc(0,idle,0);
    glutMainLoop();
    return 0;
}
#endif
