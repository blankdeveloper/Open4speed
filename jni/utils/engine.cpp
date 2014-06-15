//---------------------------------------------------------gggggg-------------------------------
/**
 * \file       engine.cpp
 * \author     Vonasek Lubos
 * \date       2014/01/05
 * \brief      Common engine functions used in program.
*/
//----------------------------------------------------------------------------------------

#include <cstdlib>
#include "interfaces/model.h"
#include "input/airacer.h"
#include "input/aitraffic.h"
#include "loaders/pngloader.h"
#include "utils/engine.h"
#include "utils/io.h"
#include "utils/math.h"
#include "utils/switch.h"
#include "common.h"

struct Dynamic {
    float* vertices;
    float* coords;
    int count;
    int frame;
};

int currentFrame = 0;                        ///< Frame index
const int effLen = 6;                        ///< Length of water effect
Dynamic eff[effLen];                         ///< 3D water effect object
model* arrow = 0;                            ///< GPS arrow model
model* water = 0;                            ///< Water effect model

/**
 * @brief displayScene displaies race scene
 * @param renderer is instance of renderer
 * @param gui is instance of menu
 * @param track is instance of track
 * @param physics is instance of physical engine
 */
void displayScene() {

    /// apply latest matrices
    matrixLock = true;
    for (int i = carCount - 1; i >= 0; i--) {
        allCar[i]->updateMatrices();
    }
    matrixLock = false;

    /// update camera direction
    if (direction * 180 / 3.14 - allCar[cameraCar]->rot > 180)
        direction -= 6.28;
    else if (direction * 180 / 3.14 - allCar[cameraCar]->rot < -180)
        direction += 6.28;
    float gap = allCar[cameraCar]->rot * 3.14 / 180.0 - direction;
    if (active) {
        if (allCar[cameraCar]->control->getDistance() < 0) {
            direction += gap / 2.0;
        } else {
            direction += gap / 15.0;
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

    xrenderer->rtt[xrenderer->oddFrame]->bindFBO();
    xrenderer->rtt[xrenderer->oddFrame]->clear(true);

    xrenderer->oddFrame = !xrenderer->oddFrame;
    xrenderer->frame++;
    if (xrenderer->frame > 1000) {
        xrenderer->frame = 0;
    }

    /// set camera
    float view = allCar[cameraCar]->getView();
    if (allCar[cameraCar]->control->getDistance() < 0) {
        view = maximalPerspective;
    }
    xrenderer->perspective(view, aspect, 0.1, viewDistance);
    xrenderer->pushMatrix();
    float cameraX = allCar[cameraCar]->transform->value[12] - sin(direction) * allCar[cameraCar]->control->getDistance() * 2 / (view / 90);
    float cameraY = allCar[cameraCar]->transform->value[13] + allCar[cameraCar]->control->getDistance() / 2 / (view / 90);
    float cameraZ = allCar[cameraCar]->transform->value[14] - cos(direction) * allCar[cameraCar]->control->getDistance() * 2 / (view / 90);
    xrenderer->lookAt(cameraX,cameraY,cameraZ,cameraX + sin(direction),cameraY + sin(-10 * 3.14 / 180),cameraZ + cos(direction),0,1,0);

    /// set light
    xrenderer->light.u_light_diffuse = glm::vec4(2000.0, 2000.0, 1500.0, 0);
    xrenderer->light.u_light_cut = cos(90.0 * 3.14 / 180.0);
    xrenderer->light.u_light_spot_eff = 5.0;
    xrenderer->light.u_light_att = glm::vec4(1.0, 5.0, 100.0, 0);
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

    /// render track
    xrenderer->enable[1] = false;
    xrenderer->renderModel(trackdata);
    if (trackdata2 != 0) {
        xrenderer->renderModel(trackdata2);
    }

    /// render skydome
    xrenderer->pushMatrix();
    xrenderer->translate(cameraX, 0, cameraZ);
    xrenderer->scale(viewDistance * 0.9);
    xrenderer->renderModel(skydome);
    xrenderer->popMatrix();

    /// render cars
    xrenderer->enable[2] = false;
    for (int i = carCount - 1; i >= 0; i--) {

        /// find nearest light
        xrenderer->light.u_nearest1 = glm::vec4(99999,99999,99999,1);
        xrenderer->model_position = glm::vec4(allCar[i]->transform->value[12], allCar[i]->transform->value[13], allCar[i]->transform->value[14], 1);
        for (int j = 0/*!lamp[(xrenderer->frame/2) % 100]*/; j < trackdata->edgesCount; j++) {
            for (unsigned int x = 0; x < trackdata->edges[j].size() / 2; x++) {
                edge e = trackdata->edges[j][x];
                glm::vec4 pos = glm::vec4(e.bx, e.by, e.bz, 1);
                if (glm::length(xrenderer->model_position - pos) < glm::length(xrenderer->model_position - xrenderer->light.u_nearest1)) {
                    xrenderer->light.u_nearest1 = pos;
                }
            }
        }
        xrenderer->light.u_nearest1 = xrenderer->view_matrix * xrenderer->light.u_nearest1;

        ///render car skin
        xrenderer->pushMatrix();
        xrenderer->multMatrix(allCar[i]->transform[0].value);
        xrenderer->translate(0, allCar[i]->wheelY, 0);
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

    // update water
    eff[currentFrame].count = 0;
    for (int i = carCount - 1; i >= 0; i--) {
        for (int j = 1; j <= 4; j++) {
            if (active) {
                float effect = fmin(fmax(fabs(allCar[i]->speed * gap) * 0.003f, fabs(allCar[i]->speed - allCar[i]->lspeed) * 0.01f), 0.1f);
                for (int k = 0; k < fabs(allCar[i]->speed) * effect - 5; k++) {
                    float x = allCar[i]->transform[j].value[12] + (rand() % 50 - 25) * 0.01f + sin(allCar[i]->rot * 3.14 / 180) * k * 0.1f;
                    float y = allCar[i]->transform[j].value[13] - 0.2f;
                    float z = allCar[i]->transform[j].value[14] + (rand() % 50 - 25) * 0.01f + cos(allCar[i]->rot * 3.14 / 180) * k * 0.1f;
                    float s = 2;
                    if (allCar[i]->speed > 150) {
                        s = allCar[i]->speed / 75.0f;
                    }
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

    /// render smoke effects
    if (water == 0) {
        water = getModel("gfx/water.o4s", false);
        for (int i = 0; i < effLen; i++) {
            eff[i] = *(new Dynamic());
            eff[i].vertices = new float[4095 * 3];
            eff[i].coords = new float[4095 * 2];
            eff[i].count = 0;
        }
    }
    for (int k = 0; k < effLen; k++) {
        if (active || (k != (currentFrame + effLen - 1) % effLen)) {
            water->models[0].texture2D->setFrame(eff[k].frame);
            xrenderer->renderDynamic(eff[k].vertices, eff[k].coords, water->models[0].material, water->models[0].texture2D, eff[k].count / 3);
        }
    }

    // render RTT
    xrenderer->rtt[xrenderer->oddFrame]->drawOnScreen(xrenderer->scene_shader);

    /// render GPS arrow
    /*xrenderer->pushMatrix();
    float a = angle(allCar[cameraCar]->currentEdge, allCar[cameraCar]);
    xrenderer->translate(allCar[cameraCar]->x, allCar[cameraCar]->y + allCar[cameraCar]->control->getDistance() + 0.5, allCar[cameraCar]->z);
    xrenderer->scale(allCar[cameraCar]->control->getDistance());
    if (a != 0)
        xrenderer->rotateY(a * 180 / 3.14);
    xrenderer->renderModel(arrow);
    xrenderer->popMatrix();*/
    xrenderer->popMatrix();


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
void loadScene(std::vector<char*> *atributes) {

    /// clear previous scene
    if (trackdata != 0) {
        while(!trackdata->lightmaps.empty()) {
            trackdata->lightmaps[0]->pointerDecrease();
            trackdata->lightmaps.erase(trackdata->lightmaps.begin());
        }
        for (int i = 0; i < carCount; i++)
            delete allCar[i];
        delete trackdata;
        delete skydome;
    }
    if (trackdata2 != 0)
        delete trackdata2;
    trackdata2 = 0;
    carCount = 0;

    /// load track
    trackdata = getModel(getConfigStr("track_model1", atributes), true);
    int trackIndex = getConfig("race_track", atributes);
    std::vector<edge> e;
    if (strlen(getConfigStr("track_model2", atributes)) > 0) {
        trackdata2 = getModel(getConfigStr("track_model2", atributes), false);
        e = trackdata2->edges[trackIndex];
    } else {
        e = trackdata->edges[trackIndex];
    }

    /// load sky
    skydome = getModel(getConfigStr("sky_model", atributes), false);
    if (arrow == 0) {
        arrow = getModel("gfx/arrow.o4s", false);
    }
    //arrow->models[0].texture2D = skydome->models[1].texture2D;

    /// load player car
    allCar[0] = new car(getInput(), &e, (*carList)[playerCar]);

    /// load race informations
    allCar[0]->lapsToGo = getConfig("laps", atributes) - 1;
    allCar[0]->finishEdge = getConfig("finish", atributes);
    allCar[0]->currentEdgeIndex = getConfig("player_start", atributes);
    allCar[0]->setStart(allCar[0]->edges[getConfig("player_start", atributes)], 0);

    /// load opponents
    opponentCount = getConfig("opponent_count", atributes);
    for (int i = 0; i < opponentCount; i++) {

        /// racer ai
        if (getConfig(getTag(i + 1, "opponent%d_type"), atributes) == 0) {
            allCar[i + 1] = new car(new airacer(), &e, (*carList)[getConfig(getTag(i + 1, "opponent%d_car"), atributes)]);
            allCar[i + 1]->finishEdge = allCar[0]->finishEdge;
            allCar[i + 1]->lapsToGo = allCar[0]->lapsToGo;
            allCar[i + 1]->setStart(allCar[i + 1]->edges[getConfig(getTag(i + 1, "opponent%d_start"), atributes)], 0);
            allCar[i + 1]->currentEdgeIndex = getConfig(getTag(i + 1, "opponent%d_start"), atributes);

        /// traffic ai
        } else {
            allCar[i + 1] = new car(new aitraffic(), &trackdata->edges[(int)getConfig(getTag(i + 1, "opponent%d_track"), atributes)],
                                    (*carList)[getConfig(getTag(i + 1, "opponent%d_car"), atributes)]);
            allCar[i + 1]->finishEdge = -1;
            allCar[i + 1]->lapsToGo = 32768;
            allCar[i + 1]->setStart(allCar[i + 1]->edges[getConfig(getTag(i + 1, "opponent%d_start"), atributes)], 3);
        }
    }

    /// load lightmaps
    if (!renderLightmap) {
        for (int i = 0; i < trackdata->getLMCount(); i++) {
            char filename[256];
            sprintf(filename, getConfigStr("lightmap", atributes), i);
            trackdata->lightmaps.push_back(getTexture(filename, 1.0));
        }
    }
}
