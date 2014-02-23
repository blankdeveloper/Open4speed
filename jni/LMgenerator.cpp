//----------------------------------------------------------------------------------------
/**
 * \file       open4speed.cpp
 * \author     Vonasek Lubos
 * \date       2014/02/23
 * \brief      Runable code of project.
*/
//----------------------------------------------------------------------------------------

#include <vector>
#include "loaders/pngloader.h"
#include "utils/io.h"
#include "utils/scripting.h"
#include "utils/switch.h"

struct LMPixel {
    float x;
    float y;
    float intensity;
};

struct LightParam {
    int begin;
    int len;
    float r;
    float g;
    float b;
};

std::vector<LMPixel> *outputVBO;
std::vector<LightParam> *lightInfo;

/**
 * @brief saveLMs save lightmap into file
 */
void saveLMs(bool final) {
    /// save lightmap into file
    for (int i = 0; i < trackdata->getLMCount(); i++) {

        /// get raw texture
        char* pixels = xrenderer->getLMPixels(i, final, final);

        /// write file
        char filename[256];
        sprintf(filename, "lightmap%d.png", i);
        writeImage(prefix(filename), rttsize, rttsize, pixels);

        /// delete arrays
        delete[] pixels;
    }
}

/**
 * @brief glRender renders lightmaps using openGL
 */
void glRender() {
    xrenderer->prepareLM(trackdata->getLMCount());
    for (int i = 0; i < trackdata->edgesCount; i++) {

        /// get light parameters
        char param[128];
        sprintf(param, "LIGHT%d", i);
        std::vector<char*> *lights = getListEx(param, "lights.ini");
        char* shadername = getConfigStr("shader", lights);

        /// light is enabled and it is not able to dynamicly disable it
        if ((strlen(shadername) > 0)  && (getConfig("blink", lights) == 0)) {
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
                xrenderer->renderLM(lightShader, true);
                printf("%d/%d\n", i, x);
            }
        }
        delete shadername;
    }
    saveLMs(true);

    /// dynamicly controlable lights
    outputVBO = new std::vector<LMPixel>[trackdata->getLMCount()];
    lightInfo = new std::vector<LightParam>[trackdata->getLMCount()];
    int* count = new int[trackdata->getLMCount()];
    for (int i = 0; i < trackdata->getLMCount(); i++) {
        count[i] = 0;
    }
    for (int i = 0; i < trackdata->edgesCount; i++) {

        /// get light parameters
        char param[128];
        sprintf(param, "LIGHT%d", i);
        std::vector<char*> *lights = getListEx(param, "lights.ini");
        char* shadername = getConfigStr("shader", lights);

        /// light is enabled and it is able to dynamicly disable it
        if ((strlen(shadername) > 0)  && (getConfig("blink", lights) > 0)) {
            shader* lightShader = getShader(shadername);
            xrenderer->light.u_light_diffuse = glm::vec4(getConfig("R", lights), getConfig("G", lights), getConfig("B", lights), 0);
            xrenderer->light.u_light_cut = cos(getConfig("cut", lights) * 3.14 / 180.0);
            xrenderer->light.u_light_spot_eff = getConfig("spot", lights);
            xrenderer->light.u_light_att = glm::vec4(getConfig("att0", lights), getConfig("att1", lights), getConfig("att2", lights), 0);
            xrenderer->light.u_near = getConfig("near", lights);

            /// apply all lights
            for (unsigned int x = 0; x < trackdata->edges[i].size() / 2; x++) {

                /// get power color
                int highIndex = 0;
                float highVal = getConfig("R", lights);
                if (highVal < getConfig("G", lights)) {
                    highIndex = 1;
                    highVal = getConfig("G", lights);
                }
                if (highVal < getConfig("B", lights)) {
                    highIndex = 2;
                    highVal = getConfig("B", lights);
                }
                xrenderer->resetLM(trackdata->getLMCount());

                edge e = trackdata->edges[i][x];
                xrenderer->light.u_light = glm::vec4(e.bx, e.by, e.bz, 1);
                xrenderer->light.u_light_dir = glm::vec4(e.bx - e.ax, e.by - e.ay, e.bz - e.az, 0);
                xrenderer->renderLM(lightShader, true);
                printf("%d/%d\n", i, x);

                /// get texel lines
                for (int y = 0; y < trackdata->getLMCount(); y++) {
                    int oldCount = count[y];
                    char* pixels = xrenderer->getLMPixels(y, true, true);
                    for (int b = 0; b < rttsize; b++) {
                        bool closed = true;
                        int lastIntensity = 0;
                        int lastA = 0;
                        for (int a = 0; a < rttsize; a++) {
                            int index = (b * rttsize + a) * 4 + highIndex;

                            /// line close
                            if (!closed && (lastIntensity != pixels[index])) {
                                outputVBO[y].push_back({lastA / (float)rttsize, b / (float)rttsize, lastIntensity / 255.0f / highVal});
                                closed = true;
                                count[y]++;
                            }

                            lastA = a;
                            lastIntensity = pixels[index];

                            /// line begin
                            if (closed) {
                                if (pixels[index] > 0) {
                                    outputVBO[y].push_back({lastA / (float)rttsize, b / (float)rttsize, lastIntensity / 255.0f / highVal});
                                    closed = false;
                                    count[y]++;
                                }
                            }
                        }

                        /// close line
                        if (!closed) {
                            outputVBO[y].push_back({lastA / (float)rttsize, b / (float)rttsize, lastIntensity / 255.0f / highVal});
                            count[y]++;
                        }
                    }


                    /// add info about light
                    lightInfo[y].push_back({oldCount, count[y] - oldCount, getConfig("R", lights), getConfig("G", lights), getConfig("B", lights)});
                }
            }
        }
        delete shadername;
    }

    /// write data
    FILE* file = fopen(prefix("lights.vbo"), "w");
    fprintf(file,"%d\n", trackdata->getLMCount());
    fprintf(file,"%d\n", lightInfo[0].size());
    for (unsigned int i = 0; i < lightInfo[0].size(); i++) {
        for (int y = 0; y < trackdata->getLMCount(); y++) {
            fprintf(file,"%d %d %f %f %f\n", lightInfo[y][i].begin, lightInfo[y][i].len, lightInfo[y][i].r, lightInfo[y][i].g, lightInfo[y][i].b);
        }
    }
    for (int y = 0; y < trackdata->getLMCount(); y++) {
        fprintf(file,"%d\n", outputVBO[y].size());
        for (unsigned int i = 0; i < outputVBO[y].size(); i++) {
            fprintf(file,"%f %f %f\n", outputVBO[y][i].x, outputVBO[y][i].y, outputVBO[y][i].intensity);
        }
    }
    fclose(file);

    printf("OK, written to %s\n", prefix("lights.vbo"));
}

/**
 * @brief display updates display
 */
void display(void) {
    if (syntaxList->empty()) {
        if (trackdata == 0) {
            syntaxList = getList("RACE2");
        } else {
            xrenderer->prepareLM(trackdata->getLMCount());
            xrenderer->renderLM(getShader("lightmap_uv"), false);
            saveLMs(false);
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
    glutPostRedisplay();
    glutTimerFunc(50,idle,0);
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

    /// init glut
    glutInit(&argc, argv);

    /// set screen mode
    glutInitWindowSize(1,1);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
    glutCreateWindow("Open4speed");

    /// set handlers
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);

    /// load menu data
    carList = getList("CARS");
    musicList = getList("MUSIC");
    syntaxList = getList("INIT");
    textList = getList("TEXTS");
    trackList = getList("TRACKS");

    /// start loop
    glutTimerFunc(0,idle,0);
    glutMainLoop();
    return 0;
}
