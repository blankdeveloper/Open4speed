//----------------------------------------------------------------------------------------
/**
 * \file       car.cpp
 * \author     Vonasek Lubos
 * \date       2014/01/05
 * \brief      Class for rendering car and updating navigation on track. It also loads
 *             informations about physical powers of current car.
*/
//----------------------------------------------------------------------------------------

#include "utils/io.h"
#include "utils/math.h"
#include "utils/switch.h"
#include "car.h"
#include "common.h"

/**
 * @brief car is constructor which loads car model
 * @param i is car control device(or program)
 * @param e is curve which car may take on
 * @param filename is path to file to load
 * @param automatic is true for automatic transmision
 */
car::car(input *i, std::vector<edge> *e, const char* filename, bool automatic) {

    /// get car atributes
    this->automatic = automatic;
#ifdef ZIP_ARCHIVE
    std::vector<char*> *atributes = getFullList(zip_fopen(APKArchive, prefix(filename), 0));
#else
    std::vector<char*> *atributes = getFullList(fopen(prefix(filename), "r"));
#endif

    edges = *(new std::vector<edge>());
    gears = new std::vector<gear>();

    /// set default values
    n2o = 150;
    onRoof = 0;
    edgeSideMove = 0;
    control = i;
    edges = *e;
    rot = 0;
    speed = 0;
    tempRot = 0;
    view = 60;
    reverse = false;
    resetAllowed = false;
    resetRequested = false;

    /// create matrices
    transform = new matrix[5];
    for (int i = 0; i < 5; i++) {
        transform[i].temp = new float[16];
        for (int j = 0; j < 16; j++)
            transform[i].temp[j] = 0;
        transform[i].temp[0] = 1;
        transform[i].temp[5] = 1;
        transform[i].temp[10] = 1;
        transform[i].temp[15] = 1;

        transform[i].value = new float[16];
        for (int j = 0; j < 16; j++)
            transform[i].value[j] = 0;
        transform[i].value[0] = 1;
        transform[i].value[5] = 1;
        transform[i].value[10] = 1;
        transform[i].value[15] = 1;
    }

    /// apply index of car
    carCount++;
    index = carCount;
    if (control != 0)
        control->index = index - 1;

    /// load models
    skin = getModel(getConfigStr("skin_model", atributes), false);
    wheel = getModel(getConfigStr("wheel_model", atributes), false);

    /// set car wheels position
    wheelX = getConfig("wheel_x", atributes);
    wheelY = getConfig("wheel_y", atributes);
    wheelZ1 = getConfig("wheel_back", atributes);
    wheelZ2 = getConfig("wheel_front", atributes);
    brakePower = getConfig("brake_power", atributes);
    steering = getConfig("steering", atributes);

    /// set gears
    currentGear = 1;
    int gearCount = getConfig("gear_count", atributes);
    for (int i = 0; i <= gearCount; i++) {
        gear g = *(new gear());
        g.min = getConfig(getTag(i,"gear%d_min"), atributes);
        g.max = getConfig(getTag(i,"gear%d_max"), atributes);
        gears->push_back(g);
    }
    gearLow = getConfig("gear_low", atributes);
    gearHigh = getConfig("gear_high", atributes);
    gearUp = getConfig("gear_up", atributes);
    gearDown = getConfig("gear_down", atributes);
    mass = getConfig("mass", atributes);
    power = getConfig("power", atributes);
    lowAspect = getConfig("low_aspect", atributes);
    acceleration = 0;
}

/**
 * @brief car destructor
 */
car::~car() {
    delete skin;
    delete wheel;
    delete control;
    for (int i = 0; i < 5; i++) {
        delete[] transform[i].temp;
        delete[] transform[i].value;
    }
    delete[] transform;
}

/**
 * @brief getView gets perspective view of car
 * @return view perspective by car speed
 */
float car::getView() {
    view = (view + (minimalPerspective + speed * perspectiveSpeedDependency)) / perspectiveSpeedFollow;
    if (view < minimalPerspective)
        view = minimalPerspective;
    if (view > maximalPerspective)
        view = maximalPerspective;
    return view;
}

/**
 * @brief setStart sets start position of car
 * @param e is curve where car starts on
 * @param sidemove is amount of car side movement(use it for rightside riding)
 */
void car::setStart(edge e, float sidemove) {

    /// error check
    if ((e.ax == e.bx) && (e.az == e.bz)) {
        return;
    }

    /// set edges
    currentEdgeOriginal = e;
    currentGoalEdge = e;
    currentEdge = sidemoveEdge(currentEdgeOriginal,sidemove);
    edgeSideMove = sidemove;

    /// set transformation
    resetAllowed = false;
    reverse = false;
    rot = (float)angle(currentEdge) * 180 / 3.14 - 180;
    lx = x = currentEdge.ax;
    ly = y = currentEdge.ay;
    lz = z = currentEdge.az;

    /// count distance from finish
    int ltg = lapsToGo;
    if (ltg < 0) {
        toFinish = 0;
        return;
    }
    edge cge = currentGoalEdge;
    toFinish = dist(cge.ax, cge.ay, cge.az, cge.bx, cge.by, cge.bz);
    while (true) {
        toFinish += dist(cge.ax, cge.ay, cge.az, cge.bx, cge.by, cge.bz);
        std::vector<int> *nEdges = nextEdge(&edges, cge);
        if (nEdges->size() > 0) {
            cge = edges[(*nEdges)[0]];
            if ((ltg > 0) & ((*nEdges)[0] == finishEdge)) {
                ltg--;
            } else if ((ltg == 0) & ((*nEdges)[0] == finishEdge)) {
                break;
            }
        } else {
            break;
        }
        delete nEdges;
    }
}


/**
 * @brief update updates car wheels state(rotation and steering)
 */
void car::update() {

    /// count speed
    lspeed = speed;
    speed = sqrt(sqr(absf(lx - x)) + sqr(absf(lz - z))) * speedAspect;

    /// update current edge for counting laps
    edge cge = currentGoalEdge;
    if (distance(currentGoalEdge, this) < 75) {
        std::vector<int> *nEdges = nextEdge(&edges, currentGoalEdge);
        if (nEdges->size() > 0) {
            if (isSame(edges[(*nEdges)[0]], edges[finishEdge])) {
                if (distance(currentGoalEdge, this) < 25) {
                    toFinish -= dist(cge.ax, cge.ay, cge.az, cge.bx, cge.by, cge.bz);
                    currentEdgeIndex = (*nEdges)[0];
                    currentGoalEdge = edges[(*nEdges)[0]];
                    if ((lapsToGo >= 0) & (isSame(edges[(*nEdges)[0]], edges[finishEdge]))) {
                        lapsToGo--;
                    }
                }
            } else {
                toFinish -= dist(cge.ax, cge.ay, cge.az, cge.bx, cge.by, cge.bz);
                currentEdgeIndex = (*nEdges)[0];
                currentGoalEdge = edges[(*nEdges)[0]];
                if ((lapsToGo >= 0) & (isSame(edges[(*nEdges)[0]], edges[finishEdge]))) {
                    lapsToGo--;
                }
            }
        }
        delete nEdges;
    }

    /// store last position
    lx = x;
    ly = y;
    lz = z;

    /// reverse gear auto change
    if (reverse & (currentGear == 1))
        currentGear = 0;
    else if (!reverse & (currentGear == 0)) {
        currentGear = 1;
    }

    /// count acceleration
    float speedDiff = (*gears)[currentGear].max - (*gears)[currentGear].min;
    float speedAspect = (speed - (*gears)[currentGear].min) / speedDiff;
    if (speedAspect > 1)
        speedAspect = 1;
    if (speedAspect < 0)
        speedAspect = 0;
    acceleration = lowAspect * power + (1 - lowAspect) * power * speedAspect;
}

/**
 * @brief updates car transformation
 */
void car::updateMatrices() {
    /// update matrices
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 16; j++) {
            transform[i].value[j] = transform[i].temp[j];
        }
    }
    rot = tempRot;


    /// reverse camera
    if (rot > 360)
        rot -= 360;
    if (reverse & (speed > 3))
        rot -= 180;
    if (rot < 0)
        rot += 360;
}

/**
 * @brief updates car sound
 */
void car::updateSound() {

    /// count engine frequency
    float speedDiff = (*gears)[currentGear].max - (*gears)[currentGear].min;
    float speedPlus = speed - (*gears)[currentGear].min;
    if (speedPlus < 0)
        speedPlus = 0;
    float engineFreq = gearLow + (gearHigh - gearLow) * speedPlus / speedDiff;

    /// automatic changing gears
    if (automatic & (currentGear >= 1)) {
        if ((currentGear > 1) & (engineFreq < gearDown))
            currentGear--;
        if (currentGear < gears->size() - 1) {
            if (engineFreq > gearUp)
                currentGear++;
        }
    } else {
        switch(control->getGearChange()) {
            case(1):
                if (currentGear < gears->size() - 1)
                    currentGear++;
                break;
            case(-1):
                if (currentGear > 1)
                    currentGear--;
                break;
        }
    }

    /// play crash
    bool brake = false;
    if (!reverse & (control->getBrake() < 1))
        brake = true;
    if (reverse & (control->getGas() < 1))
        brake = true;
    if ((lspeed * soundCrashOnSpeedChange > speed) & (speed > soundCrashMinimalSpeed) & brake) {
        crash->play(index - 1);
    }

    /// set nitro sound
    if ((control->getNitro() > 0) && (n2o > 1)) {
        noise->play(index - 1);
        noise->setFrequency(index - 1, speed * 200);
    } else {
        noise->stop(index - 1);
    }

    /// set engine sound
    if (engineFreq >= gearLow) {
        engine->play(index - 1);
        engine->setFrequency(index - 1, engineFreq * soundEngineFreqAspect);
    } else if (engineFreq < gearLow) {
        engine->setFrequency(index - 1,  gearLow * soundEngineFreqAspect);
    }

    /// set stereo sound
    float pan = gap(allCar[cameraCar],this) / 90.0;
    if (pan > 1)
        pan = 2 - pan;
    if (pan < -1)
        pan = -2 - pan;
    if (index == cameraCar + 1) {
        pan = 0;
    }
    float dist = (1 - distance(allCar[cameraCar],this) / soundMaximalDistance);
    if (dist < 0)
        dist = 0;
    dist *= dist;
    crash->setPan(index - 1, pan);
    crash->setVolume(index - 1, dist * soundVolume / 10);
    engine->setPan(index - 1, pan);
    engine->setVolume(index - 1, dist * soundVolume / 10);
    noise->setPan(index - 1, pan);
    noise->setVolume(index - 1, dist * soundVolume / 10);
}
