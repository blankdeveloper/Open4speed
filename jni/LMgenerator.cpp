//----------------------------------------------------------------------------------------
/**
 * \file       open4speed.cpp
 * \author     Vonasek Lubos
 * \date       2014/02/23
 * \brief      Runable code of project.
*/
//----------------------------------------------------------------------------------------

#include <stack>
#include <vector>
#include "loaders/pngloader.h"
#include "raycaster/octreenode.h"
#include "raycaster/triangle.h"
#include "utils/io.h"
#include "utils/math.h"
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

timespec ts_start, ts_end;          ///< Time messurement

std::vector<LMPixel> *outputVBO;
std::vector<LightParam> *lightInfo;

std::vector<unsigned char*> pixels;
std::vector<unsigned char*> uvs;
std::vector<triangle*> triangles;
std::vector<int*> lmMap;
std::vector<glm::vec3*> points;
octreenode* root;

void clearLMs() {
    for (int i = 0; i < trackdata->getLMCount(); i++) {
        for (unsigned int j = 0; j < rttsize * rttsize; j++) {
            pixels[i][j * 4 + 0] = 0;
            pixels[i][j * 4 + 1] = 0;
            pixels[i][j * 4 + 2] = 0;
            pixels[i][j * 4 + 3] = 255;
        }
    }
}

void getLMs(bool final) {
    /// get lightmap data
    for (int i = 0; i < trackdata->getLMCount(); i++) {

        /// get raw texture
        pixels.push_back(xrenderer->getLMPixels(i, final, final));
        uvs.push_back(xrenderer->getLMPixels(i, final, final));
    }
}

bool isIntersected(glm::vec3 begin, glm::vec3 end, unsigned int ignore) {
    for (unsigned int i = 0; i < triangles.size(); i++) {
        if (i != ignore) {
            if (triangles[i]->isIntersectedByRay(begin, end - begin))
                return true;
        }
    }
    return false;
}

/**
 * @brief saveLMs save lightmap into file
 */
void saveLMs(bool uv) {
    /// save lightmap into file
    for (int i = 0; i < trackdata->getLMCount(); i++) {

        /// write file
        char filename[256];
        sprintf(filename, "lightmap%d.png", i);
        if (uv)
            writeImage(prefix(filename), rttsize, rttsize, uvs[i]);
        else
            writeImage(prefix(filename), rttsize, rttsize, pixels[i]);
    }
}

struct Seed {
    int x, y, lm, value;
};

std::stack<Seed> seedStack;

bool seedTest(int x, int y, int lm) {
    if (x < 0)
        return false;
    if (y < 0)
        return false;
    if (x >= rttsize)
        return false;
    if (y >= rttsize)
        return false;
    if (lmMap[lm][y * rttsize + x] >= 0)
        return false;
    if (pixels[lm][(y * rttsize + x) * 4 + 3] < 255)
        return false;

    return true;
}

void seed(int x, int y, int lm, int value) {
    if (!seedTest(x, y, lm))
        return;
    seedStack.push({x, y, lm, value});
    while(!seedStack.empty()) {
        x = seedStack.top().x;
        y = seedStack.top().y;
        lm = seedStack.top().lm;
        value = seedStack.top().value;
        lmMap[lm][y * rttsize + x] = value;
        seedStack.pop();
        if (seedTest(x - 1, y, lm))
            seedStack.push({x - 1, y, lm, value});
        if (seedTest(x, y - 1, lm))
            seedStack.push({x, y - 1, lm, value});
        if (seedTest(x + 1, y, lm))
            seedStack.push({x + 1, y, lm, value});
        if (seedTest(x, y + 1, lm))
            seedStack.push({x, y + 1, lm, value});
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
    getLMs(true);
    saveLMs(false);

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
                    unsigned char* pixels = xrenderer->getLMPixels(y, true, true);
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

            printf("Rendering triangle UVs into lightmap...");
            clock_gettime(CLOCK_REALTIME, &ts_start);
            xrenderer->prepareLM(trackdata->getLMCount());
            xrenderer->renderLM(getShader("lightmap_uv"), false);
            clock_gettime(CLOCK_REALTIME, &ts_end);
            printf("%0.3fms\n", fabsf(ts_end.tv_nsec - ts_start.tv_nsec) * 0.000001f);

            /// create triangles for raycasting
            printf("Convert O4S model into triangles for raycasting...");
            clock_gettime(CLOCK_REALTIME, &ts_start);
            for (unsigned int i = 0; i < trackdata->models.size(); i++) {
                float* vertices = trackdata->models[i].vertices;
                float* tid = trackdata->models[i].tid;
                int triangleCount = trackdata->models[i].triangleCount[trackdata->cutX * trackdata->cutY];
                for (int j = 0; j < triangleCount; j++) {
                    /*for (int k = 0; k < 9; k++)
                      if (trackdata->models[i].vertices[j * 9 + k] == 0)
                          printf("%f %d\n", trackdata->models[i].vertices[j * 9 + k], j * 9 + k);*/
                  triangles.push_back(new triangle(
                          glm::vec3(vertices[j * 9 + 0], vertices[j * 9 + 1], vertices[j * 9 + 2]),
                          glm::vec3(vertices[j * 9 + 3], vertices[j * 9 + 4], vertices[j * 9 + 5]),
                          glm::vec3(vertices[j * 9 + 6], vertices[j * 9 + 7], vertices[j * 9 + 8]),
                          glm::vec2(tid[j * 6 + 0], tid[j * 6 + 1]),
                          glm::vec2(tid[j * 6 + 2], tid[j * 6 + 3]),
                          glm::vec2(tid[j * 6 + 4], tid[j * 6 + 5]),
                          trackdata->models[i].lmIndex));
                }
            }
            clock_gettime(CLOCK_REALTIME, &ts_end);
            printf("%0.3fms\n", fabsf(ts_end.tv_nsec - ts_start.tv_nsec) * 0.000001f);

            /// alocate triangle maps
            printf("Alocate memory for lightmap pointers to triangles...");
            clock_gettime(CLOCK_REALTIME, &ts_start);
            for (int i = 0; i < trackdata->getLMCount(); i++) {
                lmMap.push_back(new int[rttsize * rttsize]);
                for (unsigned int j = 0; j < rttsize * rttsize; j++) {
                    lmMap[i][j] = -255;
                }
            }
            clock_gettime(CLOCK_REALTIME, &ts_end);
            printf("%0.3fms\n", fabsf(ts_end.tv_nsec - ts_start.tv_nsec) * 0.000001f);

            /// fill triangle maps
            printf("Find pointers from lightmaps to triangles...");
            clock_gettime(CLOCK_REALTIME, &ts_start);
            getLMs(false);
            for (unsigned int i = 0; i < triangles.size(); i++) {
                for (int x = -1; x <= 1; x+=2)
                    for (int y = -1; y <= 1; y+=2) {
                        seed(triangles[i]->aID.x + x, triangles[i]->aID.y + y, triangles[i]->lmIndex, i);
                        seed(triangles[i]->bID.x + x, triangles[i]->bID.y + y, triangles[i]->lmIndex, i);
                        seed(triangles[i]->cID.x + x, triangles[i]->cID.y + y, triangles[i]->lmIndex, i);
                    }
            }
            clock_gettime(CLOCK_REALTIME, &ts_end);
            printf("%0.3fms\n", fabsf(ts_end.tv_nsec - ts_start.tv_nsec) * 0.000001f);

            /// fix holes in triangle maps
            printf("Fix missing pointers from lightmaps to triangles...");
            clock_gettime(CLOCK_REALTIME, &ts_start);
            for (int l = 0; l < trackdata->getLMCount(); l++) {
                for (int a = 0; a < rttsize; a++) {
                    for (int b = 0; b < rttsize; b++) {
                        if (uvs[l][(b * rttsize + a) * 4 + 3] < 128) {
                            for (int i = -1; i <= 1; i++) {
                                for (int j = -1; j <= 1; j++) {
                                    if ((a + i >= 0) && (a + i < rttsize) && (b + j >= 0) && (b + j < rttsize)) {
                                        if (uvs[l][((b + j) * rttsize + a + i) * 4 + 3] > 128) {
                                            for (int k = 0; k < 3; k++) {
                                                uvs[l][(b * rttsize + a) * 4 + k] = uvs[l][((b + j) * rttsize + a + i) * 4 + k];
                                            }
                                            lmMap[l][b * rttsize + a] = lmMap[l][(b + j) * rttsize + a + i];
                                            uvs[l][(b * rttsize + a) * 4 + 3] = 128;
                                        }
                                    }
                                    if (uvs[l][(b * rttsize + a) * 4 + 3] == 128) {
                                        break;
                                    }
                                }
                                if (uvs[l][(b * rttsize + a) * 4 + 3] == 128) {
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            clock_gettime(CLOCK_REALTIME, &ts_end);
            printf("%0.3fms\n", fabsf(ts_end.tv_nsec - ts_start.tv_nsec) * 0.000001f);

            /// create points
            printf("Count 3D coordinates of all lightmap points...");
            clock_gettime(CLOCK_REALTIME, &ts_start);
            for (int i = 0; i < trackdata->getLMCount(); i++) {
                points.push_back(new glm::vec3[rttsize * rttsize]);
                for (unsigned int j = 0; j < rttsize * rttsize; j++) {
                    if (lmMap[i][j] >= 0) {
                        points[i][j] = triangles[i][lmMap[i][j]].getPoint(uvs[i][j * 4 + 0], uvs[i][j * 4 + 1]);
                        //printf("x=%f y=%f z=%f u=%d v=%d\n", points[i][j].x, points[i][j].y, points[i][j].z, uvs[i][j * 4 + 0], uvs[i][j * 4 + 1]);
                    }
                }
            }
            clock_gettime(CLOCK_REALTIME, &ts_end);
            printf("%0.3fms\n", fabsf(ts_end.tv_nsec - ts_start.tv_nsec) * 0.000001f);

            /// triangle fill test
            printf("Visualize lightmap triangles...");
            clock_gettime(CLOCK_REALTIME, &ts_start);
            for (int i = 0; i < trackdata->getLMCount(); i++) {
                for (unsigned int j = 0; j < rttsize * rttsize; j++) {
                    if (lmMap[i][j] >= 0) {
                        /*pixels[i][j * 4 + 0] = (int)(255 * (points[i][j].x - trackdata->aabb.min.x) / (trackdata->aabb.max.x - trackdata->aabb.min.x));
                        pixels[i][j * 4 + 1] = (int)(255 * (points[i][j].y - trackdata->aabb.min.y) / (trackdata->aabb.max.y - trackdata->aabb.min.y));
                        pixels[i][j * 4 + 2] = (int)(255 * (points[i][j].z - trackdata->aabb.min.z) / (trackdata->aabb.max.z - trackdata->aabb.min.z));*/
                        pixels[i][j * 4 + 0] = (lmMap[i][j] * 126456 + 5) % 128 + 128;
                        pixels[i][j * 4 + 1] = (lmMap[i][j] * 564231 + 3) % 128 + 128;
                        pixels[i][j * 4 + 2] = (lmMap[i][j] * 789362 + 8) % 128 + 128;
                    }
                    pixels[i][j * 4 + 3] = 255;
                }
            }
            clock_gettime(CLOCK_REALTIME, &ts_end);
            printf("%0.3fms\n", fabsf(ts_end.tv_nsec - ts_start.tv_nsec) * 0.000001f);

            /// build octree
            printf("Fill octree with triangles...");
            clock_gettime(CLOCK_REALTIME, &ts_start);
            float size = fabsf(trackdata->aabb.min.x);
            if (size < fabsf(trackdata->aabb.min.y))
                size = fabsf(trackdata->aabb.min.y);
            if (size < fabsf(trackdata->aabb.min.z))
                size = fabsf(trackdata->aabb.min.z);
            if (size < fabsf(trackdata->aabb.max.x))
                size = fabsf(trackdata->aabb.max.x);
            if (size < fabsf(trackdata->aabb.max.y))
                size = fabsf(trackdata->aabb.max.y);
            if (size < fabsf(trackdata->aabb.max.z))
                size = fabsf(trackdata->aabb.max.z);
            root = new octreenode(size * 4, triangles);
            clock_gettime(CLOCK_REALTIME, &ts_end);
            printf("%0.3fms\n", fabsf(ts_end.tv_nsec - ts_start.tv_nsec) * 0.000001f);
            root->debug();

            /// render lightmaps
            /*printf("Rendering lightmaps...");
            clock_gettime(CLOCK_REALTIME, &ts_start);
            clearLMs();
            glm::vec3 light = glm::vec3(100, 100, 100);
            int p = 0;
            int m = 0;
            for (int i = 0; i < trackdata->getLMCount(); i++) {
                for (unsigned int j = 0; j < rttsize * rttsize; j++) {
                    if (lmMap[i][j] >= 0) {
                        bool v = isIntersected(light, points[i][j], lmMap[i][j]);
                        if (v)
                        {
                            p++;
                            pixels[i][j * 4 + 0] = 255;
                            pixels[i][j * 4 + 1] = 0;
                            pixels[i][j * 4 + 2] = 0;
                        }
                        else {
                            m++;
                            pixels[i][j * 4 + 0] = 0;
                            pixels[i][j * 4 + 1] = 255;
                            pixels[i][j * 4 + 2] = 0;
                        }
                    }
                    if (j % rttsize == 0)
                        printf("%d/%d: intersected=%d visible=%d\n", i, j / rttsize, p, m);
                }
            }
            clock_gettime(CLOCK_REALTIME, &ts_end);
            printf("%0.3fms\n", fabsf(ts_end.tv_nsec - ts_start.tv_nsec) * 0.000001f);*/


            /// Save lightmaps into PNGs
            printf("Save lightmaps into PNG files...");
            clock_gettime(CLOCK_REALTIME, &ts_start);
            saveLMs(false);
            clock_gettime(CLOCK_REALTIME, &ts_end);
            printf("%0.3fms\n", fabsf(ts_end.tv_nsec - ts_start.tv_nsec) * 0.000001f);
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
