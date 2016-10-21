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
#include "engine/scene.h"
#include "input/keyboard.h"

int cameraCar = 0;  ///< Car camera index
scene* scn = 0;     ///< Game scene

#ifdef ANDROID
float aliasing = 1;

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
  aliasing = alias;
  jboolean isCopy;
  setZip(env->GetStringUTFChars(apkPath, &isCopy));
  std::string filename = env->GetStringUTFChars(track, &isCopy);
  scn = new scene(filename);
}

/**
 * @brief Java_com_lvonasek_o4s_game_GameLoop_resize is resize method
 * @param env is instance of JNI
 * @param w is display width
 * @param h is display height
 */
void Java_com_lvonasek_o4s_game_GameLoop_resize( JNIEnv* env, jobject object, jint w, jint h )
{
  scn->initRenderer(w, h, aliasing);
}
/**
 * @brief Java_com_lvonasek_o4s_game_GameLoop_restart restarts player car
 * @param env is instance of JNI
 */
void Java_com_lvonasek_o4s_game_GameLoop_restart( JNIEnv* env, jobject object)
{
  scn->resetCar(0, true);
}

/**
 * @brief Java_com_lvonasek_o4s_game_GameLoop_key is key press method
 * @param env is instance of JNI
 * @param object is asset manager
 * @param code is key code
 */
void Java_com_lvonasek_o4s_game_GameLoop_key( JNIEnv* env, jobject object, jint code )
{
  keyboard::special(code, 0, 0);
}

/**
 * @brief Java_com_lvonasek_o4s_game_GameLoop_keyUp is key release method
 * @param env is instance of JNI
 * @param object is asset manager
 * @param code is key code
 */
void Java_com_lvonasek_o4s_game_GameLoop_keyUp( JNIEnv* env, jobject object, jint code )
{
  keyboard::specialUp(code, 0, 0);
}

/**
 * @brief Java_com_lvonasek_o4s_game_GameLoop_display is display method
 * @param env is instance of JNI
 * @param object is asset manager
 */
void Java_com_lvonasek_o4s_game_GameLoop_display( JNIEnv* env, jobject object )
{
  scn->render(0);
  glFinish();
}

/**
 * @brief Java_com_lvonasek_o4s_game_GameLoop_loop is loop method
 * @param env is instance of JNI
 * @param object is asset manager
 */
void Java_com_lvonasek_o4s_game_GameLoop_loop( JNIEnv* env, jobject object )
{
  scn->update();
}

/**
 * @brief Java_com_lvonasek_o4s_game_GameLoop_unload unloads resources
 * @param env is instance of JNI
 * @param object is asset manager
 */
void Java_com_lvonasek_o4s_game_GameLoop_unload( JNIEnv* env, jobject object )
{
  delete scn;
}

/**
 * @brief Java_com_lvonasek_o4s_game_GameLoop_unlock unlocks physics
 * @param env is instance of JNI
 * @param object is asset manager
 */
void Java_com_lvonasek_o4s_game_GameLoop_unlock( JNIEnv* env, jobject object )
{
    scn->setPhysicsLocked(false);
}

/**
 * @brief Java_com_lvonasek_o4s_game_GameLoop_carCount is amount of cars
 * @param env is instance of JNI
 */
jint Java_com_lvonasek_o4s_game_GameLoop_carCount( JNIEnv* env )
{
  return scn->getCarCount();
}

/**
 * @brief Java_com_lvonasek_o4s_game_GameLoop_carState is information about car
 * @param env is instance of JNI
 */
jfloat Java_com_lvonasek_o4s_game_GameLoop_carState( JNIEnv* env, jobject object, jint index, jint type )
{
    if (type == 0)
        return scn->getCar(index)->speed;
    if (type == 1)
        return scn->getCar(index)->control->getGas();
    if (type == 2)
        return scn->getCar(index)->control->getBrake();
    if (type == 3)
        return scn->getCar(index)->reverse ? 1 : -1;
    if (type == 4)
        return scn->getCar(index)->lspeed;
    if (type == 5)
        return scn->getCar(index)->gearLow;
    if (type == 6)
        return scn->getCar(index)->gearHigh;
    if (type == 7)
        return scn->getCar(index)->control->getNitro();
    if (type == 8)
        return scn->getCar(index)->gears[scn->getCar(index)->currentGear].min;
    if (type == 9)
        return scn->getCar(index)->gears[scn->getCar(index)->currentGear].max;
    if (type == 10)
        return scn->getCar(index)->n2o;
    if (type == 11)
        return distance(scn->getCar(cameraCar)->pos, scn->getCar(index)->pos);
    if (type == 12) {
        bool output = scn->getCar(index)->sndCrash;
        scn->getCar(index)->sndCrash = false;
        return output;
    }
    if (type == 13)
        return scn->getCar(index)->sndDist;
    if (type == 14)
        return scn->getCar(index)->sndEngine1;
    if (type == 15)
        return scn->getCar(index)->sndEngine2;
    if (type == 16)
        return scn->getCar(index)->sndN2O;
    if (type == 17)
        return scn->getCar(index)->sndRate;
    if (type == 18)
        return scn->getCar(index)->edges.empty() ? 9999 : scn->getCar(index)->toFinish;
    return 0;
}
}
#else

/**
 * @brief display updates display
 */
void display(void)
{
    scn->render(cameraCar);
    /// check if there is an error
    int i = glGetError();
    if (i != 0)
        printf("GL_ERROR %d\n", i);

    /// finish rendering
    glutSwapBuffers();
}

/**
 * @brief idle is non-graphical thread and it is called automatically by GLUT
 * @param v is time information
 */
void idle(int v)
{
    scn->update();

    /// call update
    glutPostRedisplay();
    glutTimerFunc(50,idle,0);
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
    if (key == 'q')
        cameraCar--;
    else if (key == 'w')
        cameraCar++;
    else if (key == 'r')
        scn->resetCar(0, false);

    if (cameraCar < 0)
        cameraCar = scn->getCarCount() - 1;
    else if (cameraCar >= (int)scn->getCarCount())
        cameraCar = 0;
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
   scn->initRenderer(w, h, 1);
}

/**
 * @brief unload unloads loaded data
 */
void unload()
{
    delete scn;
}

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
    glutInitDisplayMode(GLUT_DEPTH | GLUT_RGB);
    glutCreateWindow("Open4speed");

    /// set handlers
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);

    /// load data
    //scn = new scene("#assets/tracks/winter-day.o4scfg");
    scn = new scene("#assets/gta/gtasa.o4scfg");
    scn->setPhysicsLocked(false);

    /// start loop
    atexit(unload);
    glutTimerFunc(0,idle,0);
    glutMainLoop();
    return 0;
}
#endif
