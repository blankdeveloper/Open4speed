///----------------------------------------------------------------------------------------
/**
 * \file       scene.cpp
 * \author     Vonasek Lubos
 * \date       2016/10/14
 * \brief      Class for holding and managing scene data
**/
///----------------------------------------------------------------------------------------

#include <algorithm>
#include "engine/scene.h"
#include "input/airacer.h"
#include "input/keyboard.h"
#include "physics/bullet/bullet.h"
#include "renderers/opengl/gles20.h"
#include "renderers/opengl/glsl.h"
#include "renderers/opengl/gltexture.h"

scene* sc = 0; ///< Instance of itself for static access
pthread_mutex_t scene::dataMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t scene::loadMutex = PTHREAD_MUTEX_INITIALIZER;
glm::vec3 baseId;

bool comparator(const id3d& a, const id3d& b)
{
    glm::vec3 va = glm::vec3(a.x, a.y, a.z);
    glm::vec3 vb = glm::vec3(b.x, b.y, b.z);
    return glm::length(baseId - va) < glm::length(baseId - vb);
}

/**
 * @brief Constructor loads scene from Open4speed config file
 * @param filename is scene configuration file (o4scfg)
 */
scene::scene(std::string filename)
{
    /// set key objects
    controller = new keyboard(); //destroyed by car class
    physic = new bullet();
    xrenderer = new gles20();
    sc = this;

    /// load track
    file* f = getFile(filename);
    std::string p = f->path();
    delete f;
    loadingThreadsCount = 0;
    currentFrame = 0;
    directionY = 0;
    std::vector<std::string> atributes = getList("", filename);
    shaderPath = p + getConfigStr("shaders", atributes);
    trackPath = p + getConfigStr("track_model", atributes);
    if (fileExists(trackPath))
        trackdata = getModel(trackPath);
    else
        trackdata = 0;
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
    std::string cfgFile = p + getConfigStr("player_car", atributes);
    std::vector<std::string> carAt = getList("", cfgFile);
    model* skin = getModel(getConfigStr("skin_model", carAt));
    model* wheel = getModel(getConfigStr("wheel_model", carAt));

    addCar(new car(controller, &e, cfgFile, skin, wheel));

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
          cfgFile = p + getConfigStr("opponent" + str(i + 1) + "_car", atributes);
          carAt = getList("", cfgFile);
          skin = getModel(getConfigStr("skin_model", carAt));
          wheel = getModel(getConfigStr("wheel_model", carAt));
          addCar(new car(ai, &e, cfgFile, skin, wheel));
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
    for (int i = 0; i < WATER_EFF_LENGTH; i++)
    {
        eff[i].vertices = new float[4095 * 3];
        eff[i].coords = new float[4095 * 2];
        eff[i].count = 0;
        eff[i].frame = 0;
    }

    /// create instance of physical engine
    id3d id;
    id.x = 0;
    id.y = 0;
    id.z = 0;
    if (trackdata)
        physic->addModel(trackdata, id);
    for (unsigned int i = 0; i < getCarCount(); i++)
        physic->addCar(getCar(i));
    if (!trackdata)
        loadingLoop((void*)1);
}

/**
 * @brief model destructor
 */
scene::~scene()
{
    physic->active = false;
    pthread_mutex_lock(&sc->loadMutex);
    pthread_mutex_unlock(&sc->loadMutex);
    printf("Active threads: %d\n", loadingThreadsCount);

    while (!cars.empty())
    {
        delete cars[cars.size() - 1];
        cars.pop_back();
    }
    for (std::map<std::string, model*>::const_iterator it = models.begin(); it != models.end(); ++it)
        delete it->second;
    models.clear();
    for (std::map<std::string, shader*>::const_iterator it = shaders.begin(); it != shaders.end(); ++it)
        delete it->second;
    shaders.clear();
    for (std::map<std::string, texture*>::const_iterator it = textures.begin(); it != textures.end(); ++it)
        delete it->second;
    textures.clear();

    for (int i = 0; i < WATER_EFF_LENGTH; i++)
    {
        delete[] eff[i].vertices;
        delete[] eff[i].coords;
    }

    delete physic;
    delete xrenderer;
}

/**
 * @brief addCar adds car into scene
 * @param c is instance of new car
 */
void scene::addCar(car* c)
{
    cars.push_back(c);
    c->index = getCarCount();
}

/**
 * @brief getCar gets car from scene
 * @param index is index of car
 * @return car instance
 */
car* scene::getCar(int index)
{
    return cars[index];
}

/**
 * @brief getCarCount gets count of cars in scene
 * @return count of cars
 */
unsigned int scene::getCarCount()
{
    return cars.size();
}

/**
 * @brief getModel gets model
 * @param filename is path and name of file to load
 * @return instance of model
 */
model* scene::getModel(std::string filename)
{
    /// find previous instance
    model* instance = 0;
    filename = fixName(filename);
    pthread_mutex_lock(&dataMutex);
    if (models.find(filename) != models.end())
    {
        instance = models[filename];
        pthread_mutex_unlock(&dataMutex);
    }
    else
    {
        pthread_mutex_unlock(&dataMutex);
        instance = new model(filename, this);
        pthread_mutex_lock(&dataMutex);
        models[filename] = instance;
        pthread_mutex_unlock(&dataMutex);
    }
    return instance;
}

/**
 * @brief getShader compile shader from file
 * @param name is shader filename
 * @return instance of shader
 */
shader* scene::getShader(std::string name)
{
    char filename[1024];
    strcpy(filename, shaderPath.c_str());
    strcat(filename, name.c_str());
    strcat(filename, ".glsl");

    /// find previous instance
    name = fixName(name);
    if (shaders.find(name) != shaders.end())
    {
        shader* instance = shaders[name];
        pthread_mutex_lock(&dataMutex);
        instance->instanceCount++;
        pthread_mutex_unlock(&dataMutex);
        return instance;
    }

    std::vector<std::string> vert_atributes = getList("VERT", filename);
    std::vector<std::string> frag_atributes = getList("FRAG", filename);

    /// create shader from code
    shader* instance = new glsl(vert_atributes, frag_atributes);
    pthread_mutex_lock(&dataMutex);
    shaders[name] = instance;
    pthread_mutex_unlock(&dataMutex);
    return instance;
}

/**
 * @brief getTexture gets texture
 * @param filename is filename of texture
 * @return texture instance
 */
texture* scene::getTexture(std::string filename)
{
    filename = fixName(filename);

    /// find previous instance
    pthread_mutex_lock(&dataMutex);
    bool found = textures.find(filename) != textures.end();
    pthread_mutex_unlock(&dataMutex);
    if (found)
    {
        pthread_mutex_lock(&dataMutex);
        texture* instance = textures[filename];
        instance->instanceCount++;
        pthread_mutex_unlock(&dataMutex);
        return instance;
    }

    /// create new instance
    if (strcmp(getExtension(filename).c_str(), "png") == 0)
    {
      texture* instance = new gltexture(texture::loadPNG(getFile(filename)));
      pthread_mutex_lock(&dataMutex);
      textures[filename] = instance;
      pthread_mutex_unlock(&dataMutex);
      return instance;
    } else if (getExtension(filename)[0] == 'p')
    {
        /// get animation frame count
        std::string ext = getExtension(filename);
        int count = (ext[1] - '0') * 10 + ext[2] - '0';
        char file[255];
        strcpy(file, filename.c_str());

        /// load all sequence images
        std::vector<texture*> anim;
        for (int i = 0; i <= count; i++)
        {
            file[strlen(file) - 1] = i % 10 + '0';
            file[strlen(file) - 2] = i / 10 + '0';
            texture* instance = new gltexture(texture::loadPNG(getFile(file)));
            anim.push_back(instance);
        }

        texture* instance = new gltexture(anim);
        pthread_mutex_lock(&dataMutex);
        textures[filename] = instance;
        pthread_mutex_unlock(&dataMutex);
        return instance;
    }
    loge("Unsupported texture", filename);
    exit(1);
}

/**
 * @brief getTexture gets texture
 * @param r is amount of red from 0 to 1
 * @param g is amount of green from 0 to 1
 * @param b is amount of blue from 0 to 1
 * @return texture instance
 */
texture* scene::getTexture(float r, float g, float b)
{
    char filename[256];
    sprintf(filename, "%d %d %d", (int)(r * 255.0f), (int)(g * 255.0f), (int)(b * 255.0f));

    /// find previous instance
    pthread_mutex_lock(&dataMutex);
    bool found = textures.find(filename) != textures.end();
    pthread_mutex_unlock(&dataMutex);
    if (found)
    {
        pthread_mutex_lock(&dataMutex);
        texture* instance = textures[filename];
        instance->instanceCount++;
        pthread_mutex_unlock(&dataMutex);
        return instance;
    }

    texture* instance = new gltexture(texture::createRGB(1, 1, r, g, b));
    pthread_mutex_lock(&dataMutex);
    textures[filename] = instance;
    pthread_mutex_unlock(&dataMutex);
    return instance;
}

/**
 * @brief initRenderer inits renderer
 * @param w is screen width
 * @param h is screen height
 * @param a is screen aliasing(reducing resolution)
 */
void scene::initRenderer(int w, int h, float a)
{
    aspect = (float) w/(float) h;
    xrenderer->init(w, h, a);
}

/**
 * @brief render shows scene
 * @param cameraCar is index of car which should be traced by camera
 */
void scene::render(int cameraCar)
{
    setCamera(cameraCar);
    xrenderer->rtt(true);

    /// render skydome
    xrenderer->pushMatrix();
    xrenderer->translate(camera.x, camera.y - 50, camera.z);
    xrenderer->scale(viewDistance * 2 - 50);
    xrenderer->renderModel(skydome);
    xrenderer->popMatrix();

    /// render track
    xrenderer->enable[1] = false;
    if (trackdata)
        xrenderer->renderModel(trackdata);
    else
    {
        // start loading thread
        bool found = lastUpdate == getVisibility(false)[0];
        if (pthread_mutex_trylock(&loadMutex) == 0)
        {
            if (!found)
            {
                struct thread_info *tinfo = 0;
                pthread_create(&threadId, NULL, loadingLoop, tinfo);
                loadingThreadsCount++;
            } else
                pthread_mutex_unlock(&loadMutex);
        }

        // render culled data
        std::vector<id3d> renderId = getVisibility(true);
        pthread_mutex_lock(&dataMutex);
        for (std::vector<id3d>::const_iterator it = renderId.begin(); it != renderId.end(); ++it)
            if(trackdataCulled.find(*it) != trackdataCulled.end())
                xrenderer->renderModel(trackdataCulled[*it]);
        pthread_mutex_unlock(&dataMutex);
    }

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
    for (int k = 0; k < WATER_EFF_LENGTH; k++)
    {
        if (physic->active || (k != (currentFrame + WATER_EFF_LENGTH - 1) % WATER_EFF_LENGTH))
        {
            water->models[0].texture2D->setFrame(eff[k].frame);
            xrenderer->renderDynamic(eff[k].vertices, 0, eff[k].coords, water->models[0].material, water->models[0].texture2D, eff[k].count / 3);
        }
    }
    xrenderer->popMatrix();
    // render RTT
    xrenderer->rtt(false);
}

/**
 * @brief update updates scene physics
 */
void scene::update()
{
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
            c->update(distance(getCar(0)->pos, c->pos));

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
        id3d id;
        id.x = 0;
        id.y = 0;
        id.z = 0;
        if (trackdata)
        {
            for (unsigned int i = 0; i < trackdata->models.size(); i++)
                if (trackdata->models[i].dynamic)
                    physic->getTransform(trackdata->models[i].dynamicID, trackdata->models[i].dynamicMat, id);
        }

        /// update scene
        physic->updateWorld();
    }

    // update water
    eff[currentFrame].count = 0;
    for (int i = getCarCount() - 1; i >= 0; i--)
    {
        float g = getCar(i)->rot * 3.14 / 180.0 - directionY;
        float effect = fabs(getCar(i)->speed) * fabs(getCar(i)->speed * (g + getCar(i)->control->getBrake() * 5.0)) * 0.001f;
        effect = fmax(effect, effect * 0.05 + getCar(i)->prevEffect * 0.95);
        getCar(i)->prevEffect = effect;
        for (int j = 1; j <= 2; j++)
        {
            if (physic->active)
            {
                for (int k = 0; k < fmin(effect - 5, 5); k++)
                {
                    float x = getCar(i)->transform[j].value[12] + (rand() % 50 - 25) * 0.03f + sin(getCar(i)->rot * 3.14 / 180) * k * 0.03f;
                    float y = getCar(i)->transform[j].value[13] - 0.1f;
                    float z = getCar(i)->transform[j].value[14] + (rand() % 50 - 25) * 0.03f + cos(getCar(i)->rot * 3.14 / 180) * k * 0.03f;
                    for (unsigned int l = 0; l < water->models[0].count * 3; l++)
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
    for (int k = 0; k < WATER_EFF_LENGTH; k++)
        eff[k].frame++;
    eff[currentFrame].frame = 0;
    currentFrame++;
    if (currentFrame >= WATER_EFF_LENGTH)
        currentFrame = 0;
}

/**
 * @brief getVisibility returns ids for culled scene
 * @param directional is true to return parts in direction only
 * @return ids of culled scene
 */
std::vector<id3d> scene::getVisibility(bool directional)
{
    pthread_mutex_lock(&sc->dataMutex);
    std::vector<id3d> output;
    int steps = 3;
    int cx = camera.x / CULLING_DST;
    int cy = camera.y / CULLING_DST;
    int cz = camera.z / CULLING_DST;
    id3d id;
    id.x = cx;
    id.y = cy;
    id.z = cz;
    if( directional )
    {
        float m = 0.5f;
        glm::vec3 dir = glm::normalize(direction);
        for (int x = (dir.x < m ? -steps : -1); x <= (dir.x > -m ? steps : 1); x++)
            for (int y = (dir.y < m ? -steps : -1); y <= (dir.y > -m ? steps : 1); y++)
                for (int z = (dir.z < m ? -steps : -1); z <= (dir.z > -m ? steps : 1); z++)
                {
                    id.x = cx + x;
                    id.y = cy + y;
                    id.z = cz + z;
                    output.push_back(id);
                }
    }
    else
    {
        for (int x = -steps; x <= steps; x++)
            for (int y = -steps; y <= steps; y++)
                for (int z = -steps; z <= steps; z++)
                {
                    id.x = cx + x;
                    id.y = cy + y;
                    id.z = cz + z;
                    output.push_back(id);
                }
    }
    baseId.x = cx;
    baseId.y = cy;
    baseId.z = cz;
    std::sort(output.begin(), output.end(), comparator);
    pthread_mutex_unlock(&sc->dataMutex);
    return output;
}

/**
 * @brief setCamera sets camera in scene by car
 * @param cameraCar is index of car which should be traced by camera
 */
void scene::setCamera(int cameraCar)
{
    pthread_mutex_lock(&dataMutex);
    /// update camera direction
    if (directionY * 180 / 3.14 - getCar(cameraCar)->rot > 180)
        directionY -= 6.28;
    else if (directionY * 180 / 3.14 - getCar(cameraCar)->rot < -180)
        directionY += 6.28;
    float g = getCar(cameraCar)->rot * 3.14 / 180.0 - directionY;
    if (physic->active)
    {
        if (getCar(cameraCar)->control->getDistance() < 0)
            directionY += g / 2.0;
        else
            directionY += g / 15.0;
    } else
    {
        directionY += 0.01;
        if (directionY > 6.28)
            directionY -= 6.28;
    }

    /// fix camera direction
    int rot = getCar(cameraCar)->rot * 1000;
    rot += 720 * 1000;
    rot = rot % 360000;
    getCar(cameraCar)->rot = rot / 1000.0f;

    /// set camera
    float view = getCar(cameraCar)->getView();
    xrenderer->perspective(view, aspect, 0.5, viewDistance);
    xrenderer->pushMatrix();
    float x = getCar(cameraCar)->transform->value[12];
    float y = getCar(cameraCar)->transform->value[13];
    float z = getCar(cameraCar)->transform->value[14];
    //need for speed style
    x -= sin(directionY) * getCar(cameraCar)->control->getDistance() * 2.5f / (view / 90);
    y += fmax(1.0, getCar(cameraCar)->control->getDistance()) / (view / 90);
    z -= cos(directionY) * getCar(cameraCar)->control->getDistance() * 2.5f / (view / 90);
    glm::vec3 center = glm::vec3(x + sin(directionY) * 100.0f, y, z + cos(directionY) * 100.0f);
    camera = glm::vec3(x - sin(directionY) * 0.1f, y + 0.5f, z - cos(directionY) * 0.1f);
    xrenderer->lookAt(camera, center, glm::vec3(0, 1, 0));
    direction = glm::normalize(center - camera);
    pthread_mutex_unlock(&dataMutex);
}

/**
 * @brief loadingLoop is cycle for loading data on background
 * @param ptr is 1 for non pthread calling
 * @return 0
 */
void* scene::loadingLoop(void *ptr)
{
    // mark everything for deleting
    std::map<id3d, bool> toDeleteId;
    pthread_mutex_lock(&sc->dataMutex);
    for (std::map<id3d, model*>::const_iterator it = sc->trackdataCulled.begin(); it != sc->trackdataCulled.end(); ++it)
        if (it->second)
        {
            it->second->toDelete = true;
            toDeleteId[it->first] = true;
        }
    pthread_mutex_unlock(&sc->dataMutex);

    // load culled data
    std::vector<id3d> loadId = sc->getVisibility(false);
    for (std::vector<id3d>::const_iterator it = loadId.begin(); it != loadId.end(); ++it)
    {
        pthread_mutex_lock(&sc->dataMutex);
        bool found = sc->trackdataCulled.find(*it) != sc->trackdataCulled.end();
        pthread_mutex_unlock(&sc->dataMutex);
        if (!found)
        {
            std::string name = sc->id2str(*it);
            if (fileExists(name))
            {
                model* m = sc->getModel(name);
                pthread_mutex_lock(&sc->dataMutex);
                sc->trackdataCulled[*it] = m;
                pthread_mutex_unlock(&sc->dataMutex);
                sc->physic->addModel(m, *it);
                found = true;
            }
        }
        if (found)
        {
            pthread_mutex_lock(&sc->dataMutex);
            sc->trackdataCulled[*it]->toDelete = false;
            toDeleteId[*it] = false;
            pthread_mutex_unlock(&sc->dataMutex);
        }
    }

    // remove unused models
    pthread_mutex_lock(&sc->dataMutex);
    for (std::map<id3d, bool>::const_iterator it = toDeleteId.begin(); it != toDeleteId.end(); ++it)
    {
        if (it->second)
        {
            sc->trackdataCulled.erase(it->first);
            sc->physic->removeModel(it->first);
        }
    }
    std::vector<std::string> toDelete;
    for (std::map<std::string, model*>::const_iterator it = sc->models.begin(); it != sc->models.end(); ++it)
    {
        if (it->second->toDelete)
        {
            delete it->second;
            toDelete.push_back(it->first);
        }
    }
    for (std::vector<std::string>::const_iterator it = toDelete.begin(); it != toDelete.end(); ++it)
        sc->models.erase(*it);
    toDelete.clear();
    for (std::map<std::string, shader*>::const_iterator it = sc->shaders.begin(); it != sc->shaders.end(); ++it)
    {
        if (it->second->instanceCount == 0)
        {
            delete it->second;
            toDelete.push_back(it->first);
        }
    }
    for (std::vector<std::string>::const_iterator it = toDelete.begin(); it != toDelete.end(); ++it)
        sc->shaders.erase(*it);
    toDelete.clear();
    for (std::map<std::string, texture*>::const_iterator it = sc->textures.begin(); it != sc->textures.end(); ++it)
    {
        if (it->second->instanceCount == 0)
        {
            delete it->second;
            toDelete.push_back(it->first);
        }
    }
    for (std::vector<std::string>::const_iterator it = toDelete.begin(); it != toDelete.end(); ++it)
        sc->textures.erase(*it);

    // set indicator what was updated
    sc->lastUpdate = loadId[0];
    pthread_mutex_unlock(&sc->dataMutex);
    if (ptr == 0)
    {
        pthread_t threadId = sc->threadId;
        sc->loadingThreadsCount--;
        pthread_mutex_unlock(&sc->loadMutex);
        pthread_detach(threadId);
    }
    return 0;
}
