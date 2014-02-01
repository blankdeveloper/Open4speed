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
 * @brief display updates display
 */
void display(void) {

    if (syntaxList->empty()) {
        if (trackdata == 0) {
            syntaxList = getList("RACE1");
        } else {
            xrenderer->prepareLM(trackdata->getLMCount());
            for (int i = 0; i < trackdata->edgesCount; i++) {

                /// get light parameters
                char param[128];
                sprintf(param, "LIGHT%d", i);
                std::vector<char*> *lights = getListEx(param, "lights.ini");
                char* shadername = getConfigStr("shader", lights);

                /// light is enabled
                if (strlen(shadername) > 0) {
                    shader* lightShader = getShader(shadername);
                    xrenderer->light.u_light_diffuse = glm::vec4(getConfig("R", lights), getConfig("G", lights), getConfig("B", lights), 0);
                    xrenderer->light.u_light_cut = cos(getConfig("cut", lights) * 3.14 / 180.0);
                    xrenderer->light.u_light_spot_eff = getConfig("spot", lights);
                    xrenderer->light.u_light_att = glm::vec4(getConfig("att0", lights), getConfig("att1", lights), getConfig("att2", lights), 0);
                    xrenderer->light.u_near = getConfig("near", lights);

                    /// apply all lights
                    for (unsigned int x = 0; x < trackdata->edges[i].size() / 2; x++) {
                        edge e = trackdata->edges[i][x];
                        xrenderer->light.u_light = glm::vec4(e.bx, e.by, e.bz, 1);
                        xrenderer->light.u_light_dir = glm::vec4(e.bx - e.ax, e.by - e.ay, e.bz - e.az, 0);
                        xrenderer->renderLMLight(lightShader);
                        printf("%d/%d\n", i, x);
                    }
                }
                delete shadername;
            }

            xrenderer->saveLMs();
            printf("OK\n");
            exit(0);
        }
    }

    /// apply all scripts
    scriptLoop();

    /// update menu
    updateMenu();

    /// check if there is an error
    int i = glGetError();
    if (i != 0) {
        printf("GL_ERROR %d\n", i);
    }

    /// finish rendering
    glutSwapBuffers();
}

/**
 * @brief idle is non-graphical thread and it is called automatically by GLUT
 * @param v is time information
 */
void idle(int v) {
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
    renderLightmap = true;

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
    glutInitWindowSize(1,1);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
    glutCreateWindow("Open4speed");

    /// set handlers
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
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
