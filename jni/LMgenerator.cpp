//----------------------------------------------------------------------------------------
/**
 * \file       open4speed.cpp
 * \author     Vonasek Lubos
 * \date       2014/03/02
 * \brief      Runable code of project.
*/
//----------------------------------------------------------------------------------------

#include <stack>
#include <vector>
#include "loaders/pngloader.h"
#include "raycaster/octreenode.h"
#include "raycaster/triangle.h"
#include "raycaster/utils.h"
#include "utils/io.h"
#include "utils/math.h"
#include "utils/scripting.h"
#include "utils/switch.h"

std::vector<unsigned char*> pixels;
std::vector<unsigned char*> uvs;
std::vector<triangle*> triangles;
std::vector<long*> lmMap;
octreenode* root;

void clearLMs() {
    for (int i = 0; i < trackdata->getLMCount(); i++) {
        for (unsigned int j = 0; j < rttsize * rttsize * 4; j++) {
            pixels[i][j] = 0;
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

/**
 * @brief saveLMs save lightmap into file
 */
void saveLMs() {
    /// save lightmap into file
    for (int i = 0; i < trackdata->getLMCount(); i++) {

        /// write file
        char filename[256];
        sprintf(filename, "lightmap%d.png", i);
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
 * @brief display updates display
 */
void display(void) {
    if (syntaxList->empty()) {
        if (trackdata == 0) {
            syntaxList = getList("RACE2");
        } else {

            printf("Rendering triangle UVs into lightmap...");
            startTimer();
            xrenderer->prepareLM(trackdata->getLMCount());
            xrenderer->renderLM(getShader("lightmap_uv"), false);
            stopTimer();

            /// create triangles for raycasting
            printf("Converting O4S model into triangles for raycasting...");
            startTimer();
            long index = 0;
            char* string = new char[1024];
            for (unsigned int i = 0; i < trackdata->models.size(); i++) {
                float* vertices = trackdata->models[i].vertices;
                float* normals = trackdata->models[i].normals;
                float* coords = trackdata->models[i].coords;
                float* tid = trackdata->models[i].tid;
                int triangleCount = trackdata->models[i].triangleCount[trackdata->cutX * trackdata->cutY];
                float x = trackdata->models[i].reg->min.x;
                float y = trackdata->models[i].reg->min.y;
                float z = trackdata->models[i].reg->min.z;
                Texture* txt = 0;
                if (trackdata->models[i].texture2D->transparent)
                  txt = loadPNG(trackdata->models[i].texture2D->texturename);
                strcpy(string, trackdata->models[i].texture2D->texturename);
                strcat(string, "-map");
                Texture* txtmap = loadPNG(string);
                for (int j = 0; j < triangleCount; j++) {
                  triangles.push_back(new triangle(
                                          glm::vec3(vertices[j * 9 + 0]+x, vertices[j * 9 + 1]+y, vertices[j * 9 + 2]+z),
                                          glm::vec3(vertices[j * 9 + 3]+x, vertices[j * 9 + 4]+y, vertices[j * 9 + 5]+z),
                                          glm::vec3(vertices[j * 9 + 6]+x, vertices[j * 9 + 7]+y, vertices[j * 9 + 8]+z),
                                          glm::vec2(tid[j * 6 + 0], tid[j * 6 + 1]),
                                          glm::vec2(tid[j * 6 + 2], tid[j * 6 + 3]),
                                          glm::vec2(tid[j * 6 + 4], tid[j * 6 + 5]),
                                          glm::vec3(normals[j * 9 + 0], normals[j * 9 + 1], normals[j * 9 + 2]),
                                          glm::vec3(normals[j * 9 + 3], normals[j * 9 + 4], normals[j * 9 + 5]),
                                          glm::vec3(normals[j * 9 + 6], normals[j * 9 + 7], normals[j * 9 + 8]),
                                          glm::vec2(coords[j * 6 + 0], coords[j * 6 + 1]),
                                          glm::vec2(coords[j * 6 + 2], coords[j * 6 + 3]),
                                          glm::vec2(coords[j * 6 + 4], coords[j * 6 + 5]),
                                          trackdata->models[i].lmIndex, index++, txt, txtmap));
                }
            }
            stopTimer();

            /// alocate triangle maps
            printf("Alocating memory for lightmap pointers to triangles...");
            startTimer();
            for (int i = 0; i < trackdata->getLMCount(); i++) {
                lmMap.push_back(new long[rttsize * rttsize]);
                for (unsigned int j = 0; j < rttsize * rttsize; j++) {
                    lmMap[i][j] = -255;
                }
            }
            stopTimer();

            /// fill triangle maps
            printf("Finding pointers from lightmaps to triangles...");
            startTimer();
            getLMs(false);
            for (unsigned long i = 0; i < triangles.size(); i++) {
                for (int x = -1; x <= 1; x++)
                    for (int y = -1; y <= 1; y++) {
                        seed(triangles[i]->aID.x + x, triangles[i]->aID.y + y, triangles[i]->lmIndex, triangles[i]->tIndex);
                        seed(triangles[i]->bID.x + x, triangles[i]->bID.y + y, triangles[i]->lmIndex, triangles[i]->tIndex);
                        seed(triangles[i]->cID.x + x, triangles[i]->cID.y + y, triangles[i]->lmIndex, triangles[i]->tIndex);
                    }
            }
            stopTimer();

            /// create points
            printf("Counting 3D coordinates of all lightmap points...");
            startTimer();
            for (int i = 0; i < trackdata->getLMCount(); i++) {
                for (int a = 0; a < rttsize; a++) {
                    for (int b = 0; b < rttsize; b++) {
                        int j = b * rttsize + a;
                        if (lmMap[i][j] >= 0) {
                            triangles[lmMap[i][j]]->addLMPoint(uvs[i][j * 4 + 0], uvs[i][j * 4 + 1], glm::ivec2(a, b));
                        }
                    }
                }
            }
            stopTimer();

            /// build octree
            printf("Filling octree with triangles...");
            startTimer();
            root = new octreenode(&trackdata->aabb, triangles);
            stopTimer();
            root->debug(false);

            /// render point lights into lightmaps
            printf("Rendering point lights into lightmaps...");
            startTimer();
            clearLMs();
            unsigned long count = 0;
            for (int lightGroup = 0; lightGroup < trackdata->edgesCount; lightGroup++) {
                /// get light parameters
                char param[128];
                sprintf(param, "LIGHT%d", lightGroup);
                std::vector<char*> *lights = getListEx(param, "lights.ini");
                char* shadername = getConfigStr("shader", lights);
                /// light is enabled and it is not able to dynamicly disable it
                if (strlen(shadername) > 0) {
                    count+=trackdata->edges[lightGroup].size() / 2;
                }
                delete[] shadername;
            }
            float status = 0;
            for (int lightGroup = 0; lightGroup < trackdata->edgesCount; lightGroup++) {
                /// get light parameters
                char param[128];
                sprintf(param, "LIGHT%d", lightGroup);
                std::vector<char*> *lights = getListEx(param, "lights.ini");
                char* shadername = getConfigStr("shader", lights);
                /// light is enabled and it is not able to dynamicly disable it
                if (strlen(shadername) > 0) {
                    xrenderer->light.u_light_diffuse = glm::vec4(getConfig("R", lights), getConfig("G", lights), getConfig("B", lights), 0);
                    xrenderer->light.u_light_cut = cos(getConfig("cut", lights) * 3.14 / 180.0);
                    xrenderer->light.u_light_att = glm::vec4(getConfig("att0", lights), getConfig("att1", lights), getConfig("att2", lights), 0);

                    /// apply all lights
                    for (unsigned int x = 0; x < trackdata->edges[lightGroup].size() / 2; x++) {
                        edge e = trackdata->edges[lightGroup][x];
                        xrenderer->light.u_light = glm::vec4(e.ax, e.ay, e.az, 1);
                        xrenderer->light.u_light_dir = glm::vec4(e.bx - e.ax, e.by - e.ay, e.bz - e.az, 0);

                        // render SUN experiment
                        /*xrenderer->light.u_light = glm::vec4(100, 100, 100, 1);
                        xrenderer->light.u_light_dir = glm::vec4(0, 1, 0, 0);
                        xrenderer->light.u_light_diffuse = glm::vec4(getConfig("R", lights), getConfig("G", lights), getConfig("B", lights), 0) * 10000.0f;*/
                        /// create ray and raycast
                        glm::vec3 begin = swizle(xrenderer->light.u_light);
                        for (unsigned long i = 0; i < triangles.size(); i++) {
                            ignore1 = triangles[i]->tIndex;
                            ignore2 = triangles[i]->tIndex;
                            for (unsigned int j = 0; j < triangles[i]->points.size(); j++) {
                                glm::vec3 end = triangles[i]->points[j]->v;
                                int index = triangles[i]->points[j]->t.y * rttsize + triangles[i]->points[j]->t.x;
                                glm::vec4 color = getColor(triangles[i]->points[j], begin, end);
                                if (color.w > 0.5f) {
                                    if (!root->isIntersected(begin, end)) {
                                        pixels[triangles[i]->lmIndex][index * 4 + 0] = min(255, pixels[triangles[i]->lmIndex][index * 4 + 0] + (int)(color.x * 8.0f));
                                        pixels[triangles[i]->lmIndex][index * 4 + 1] = min(255, pixels[triangles[i]->lmIndex][index * 4 + 1] + (int)(color.y * 8.0f));
                                        pixels[triangles[i]->lmIndex][index * 4 + 2] = min(255, pixels[triangles[i]->lmIndex][index * 4 + 2] + (int)(color.z * 8.0f));
                                    }
                                }
                                pixels[triangles[i]->lmIndex][index * 4 + 3] = 255;
                            }
                            float percent = status + 100 * i / (float)triangles.size() / (float)count;
                            printf("\rRendering point lights into lightmaps...%0.3f%% done", percent);
                            fflush(stdout);
                        }
                        status += 100 / (float)count;
                        //break;
                    }
                }
                delete[] shadername;
            }
            printf("\r                                                        ");
            printf("\rRendering point lights into lightmaps...");
            stopTimer();

            /// generate area point lights
            printf("Generating point lights from area lights...");
            startTimer();
            std::vector<int> pIndex;
            std::vector<int> trIndex;
            std::vector<PLP*> plpIndex;
            for (unsigned long k = 0; k < triangles.size(); k++) {
                for (unsigned int x = 0; x < triangles[k]->points.size(); x++) {
                    index = triangles[k]->points[x]->t.y * rttsize + triangles[k]->points[x]->t.x;
                    pixels[triangles[k]->lmIndex][index * 4 + 3] = 255;
                    PLP *plp = triangles[k]->getPointLight(triangles[k]->points[x]->v);
                    if (plp->useable) {
                        pIndex.push_back(x);
                        trIndex.push_back(k);
                        plpIndex.push_back(plp);
                        //emise light from texels
                        int index = triangles[k]->points[x]->t.y * rttsize + triangles[k]->points[x]->t.x;
                        pixels[triangles[k]->lmIndex][index * 4 + 0] = (int)(plp->color.x * 255.0f);
                        pixels[triangles[k]->lmIndex][index * 4 + 1] = (int)(plp->color.y * 255.0f);
                        pixels[triangles[k]->lmIndex][index * 4 + 2] = (int)(plp->color.z * 255.0f);
                    } else {
                        delete plp;
                    }
                }
            }
            stopTimer();
            printf("Generated %d lights\n", pIndex.size());

            /// render area lights into lightmaps
            printf("Rendering area lights into lightmaps...");
            startTimer();
            float cutoff = cos(135 * 3.14 / 180.0);

            /// apply all lights
            glm::vec3 raybegin;
            glm::vec4 color;
            float att = 0.05f;
            float eff;
            int tr;
            for (unsigned long i = 0; i < triangles.size(); i++) {
                for (unsigned long k = 0; k < pIndex.size(); k++) {
                    xrenderer->light.u_light_diffuse = plpIndex[k]->color * 10.0f;
                    if (triangles[i]->countDistanceTo(triangles[trIndex[k]]) < countLightMaxDistance(att)) {
                        xrenderer->light.u_light_diffuse.w = 255;
                        xrenderer->light.u_light = plpIndex[k]->pos;
                        xrenderer->light.u_light_dir = plpIndex[k]->dir;
                        raybegin = swizle(xrenderer->light.u_light);
                        tr = trIndex[k];
                        while ((tr == trIndex[k]) && (k < pIndex.size())) {
                            ignore1 = triangles[i]->tIndex;
                            ignore2 = triangles[trIndex[k]]->tIndex;
                            for (unsigned int j = 0; j < triangles[i]->points.size(); j++) {
                                glm::vec3 rayend = triangles[i]->points[j]->v;
                                glm::vec3 raydir = rayend - raybegin;
                                glm::vec3 L = glm::normalize(-raydir);
                                eff = glm::dot(-swizle(xrenderer->light.u_light_dir), -L);
                                if (eff >= cutoff) {
                                    //diffuse light
                                    color = glm::max(glm::dot(triangles[i]->points[j]->n, L), 0.0f) * xrenderer->light.u_light_diffuse;
                                    //light attenuation
                                    color *= eff / (att * sqr(glm::length(raydir)));
                                    if (color.w > 0.5f) {
                                        if (!root->isIntersected(raybegin, rayend)) {
                                            // add to previous lightmap
                                            index = triangles[i]->points[j]->t.y * rttsize + triangles[i]->points[j]->t.x;
                                            pixels[triangles[i]->lmIndex][index * 4 + 0] = min(255, pixels[triangles[i]->lmIndex][index * 4 + 0] + (int)(color.x * 8.0f));
                                            pixels[triangles[i]->lmIndex][index * 4 + 1] = min(255, pixels[triangles[i]->lmIndex][index * 4 + 1] + (int)(color.y * 8.0f));
                                            pixels[triangles[i]->lmIndex][index * 4 + 2] = min(255, pixels[triangles[i]->lmIndex][index * 4 + 2] + (int)(color.z * 8.0f));
                                        }
                                    }
                                }
                            }
                            k++;
                        }
                    } else {
                        tr = trIndex[k];
                        while ((tr == trIndex[k]) && (k < pIndex.size())) {
                            k++;
                        }
                    }
                }
                float percent = 100 * (i + 1) / (float)(triangles.size());
                printf("\rRendering area lights lightmaps...%0.3f%% done", percent);
                fflush(stdout);
            }
            printf("\r                                                        ");
            printf("\rRendering area lights into lightmaps...");
            stopTimer();

            /// fix holes in lightmaps
            printf("Fixing holes in lightmaps...");
            startTimer();
            for (int l = 0; l < trackdata->getLMCount(); l++) {
                /// fill holes
                for (int a = 0; a < rttsize; a++) {
                    for (int b = 0; b < rttsize; b++) {
                        if (pixels[l][(b * rttsize + a) * 4 + 3] < 128) {
                            for (int i = -1; i <= 1; i++) {
                                for (int j = -1; j <= 1; j++) {
                                    if ((a + i >= 0) && (a + i < rttsize) && (b + j >= 0) && (b + j < rttsize)) {
                                        if (pixels[l][((b + j) * rttsize + a + i) * 4 + 3] > 128) {
                                            for (int k = 0; k < 3; k++) {
                                                pixels[l][(b * rttsize + a) * 4 + k] = pixels[l][((b + j) * rttsize + a + i) * 4 + k];
                                            }
                                            pixels[l][(b * rttsize + a) * 4 + 3] = 128;
                                        }
                                    }
                                    if (pixels[l][(b * rttsize + a) * 4 + 3] == 128) {
                                        break;
                                    }
                                }
                                if (pixels[l][(b * rttsize + a) * 4 + 3] == 128) {
                                    break;
                                }
                            }
                        }
                    }
                }
                /// copy pixels
                unsigned char* pixels2 = new unsigned char[rttsize * rttsize * 4];
                for (unsigned int i = 0; i < rttsize * rttsize * 4; i++) {
                    pixels2[i] = pixels[l][i];
                }
                /// blur
                for (int a = 0; a < rttsize; a++) {
                    for (int b = 0; b < rttsize; b++) {
                        for (int k = 0; k < 3; k++) {
                            int count = 1;
                            int value = pixels[l][(b * rttsize + a) * 4 + k];
                            for (int i = -1; i <= 1; i++) {
                                for (int j = -1; j <= 1; j++) {
                                    if ((b + j >= 0) && (b + j < rttsize))
                                        if ((a + i >= 0) && (a + i < rttsize))
                                            if (pixels[l][((b + j) * rttsize + a + i) * 4 + 3] > 128) {
                                                count++;
                                                value += pixels2[((b + j) * rttsize + a + i) * 4 + k];
                                            }
                                }
                            }
                            pixels[l][(b * rttsize + a) * 4 + k] = value / count;
                        }
                    }
                }
                /// fix alpha channel
                for (int a = 0; a < rttsize * rttsize; a++) {
                    pixels[l][a * 4 + 3] = 255;
                }
                delete[] pixels2;
            }
            stopTimer();

            /// Save lightmaps into PNGs
            printf("Saving lightmaps into PNG files...");
            startTimer();
            saveLMs();
            stopTimer();

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
