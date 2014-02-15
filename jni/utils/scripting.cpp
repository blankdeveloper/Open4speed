//----------------------------------------------------------------------------------------
/**
 * \file       scripting.cpp
 * \author     Vonasek Lubos
 * \date       2014/01/05
 * \brief      Script interpreter
*/
//----------------------------------------------------------------------------------------

#include <unistd.h>
#include "utils/io.h"
#include "utils/switch.h"
#include "common.h"

/**
 * @brief scriptLoop is interpreting scripts in loop(it must be called periodically)
 */
void scriptLoop() {

    /// Syntax wrapping
    if ((!busy) & (unlock < time(NULL))) {
        while (syntaxList->size() > 0) {
            char syntax[16];
            sscanf((*syntaxList)[0], "%s", &syntax[0]);
            logi("exec:", (*syntaxList)[0]);
            if (strcmp("active", syntax) == 0) {
                int value;
                sscanf((*syntaxList)[0], "%s %d", &syntax[0], &value);
                if (value == 1)
                    active = true;
                else
                    active = false;
            }

            /// Add button into GUI
            if (strcmp("addButton", syntax) == 0) {
                button b = *(new button());
                b.syntax[0] = '\0';
                b.layer = 0.96;
                char value[16];
                sscanf((*syntaxList)[0], "%s img %d %s %d %fx%f-%fx%f %s", &syntax[0], &b.image, &value[0], &b.text, &b.x, &b.y,
                       &b.width, &b.height, &b.syntax[0]);
                b.type = -1;
                if (strcmp("text", value) == 0)
                    b.type = 0;
                if (strcmp("cfg", value) == 0)
                    b.type = 1;
                if (strcmp("mon", value) == 0)
                    b.type = 2;
                buttons->push_back(b);
            }

            /// Add image into list
            if (strcmp("addImage", syntax) == 0) {
                char value[64];
                sscanf((*syntaxList)[0], "%s %s", &syntax[0], &value[0]);
                texture* image = getTexture(value, 1);
                images->push_back(image);
            }

            /// Clear all buttons from GUI
            if (strcmp("clearButtons", syntax) == 0) {
                buttons->clear();
            }

            /// Set visibility of debug informations
            if (strcmp("debug", syntax) == 0) {
                int value;
                sscanf((*syntaxList)[0], "%s %d", &syntax[0], &value);
                if (value == 1)
                    debug = true;
                else
                    debug = false;
            }

            /// Set detail level
            if (strcmp("detail", syntax) == 0) {
                if (variable == 0) {
                    antialiasing = 0.2f;
                } else if (variable == 1) {
                    antialiasing = 0.4f;
                } else if (variable == 2) {
                    antialiasing = 0.6f;
                } else if (variable == 3) {
                    antialiasing = 0.8f;
                } else if (variable == 4) {
                    antialiasing = 1.0f;
                }
                xrenderer = getRenderer();
            }

            /// Set on esc key listener
            if (strcmp("esc", syntax) == 0) {
                sscanf((*syntaxList)[0], "%s %s", &syntax[0], &esc[0]);
            }

            /// Set on finish listener
            if (strcmp("finish", syntax) == 0) {
                sscanf((*syntaxList)[0], "%s %s", &syntax[0], &finish[0]);
            }

            /// Force GUI render
            if (strcmp("forceRender", syntax) == 0) {
                /// cancel executing syntaxes
                delete[] (*syntaxList)[0];
                syntaxList->erase(syntaxList->begin());
                break;
            }

            /// GUI input animation
            if (strcmp("fxFlyIn", syntax) == 0) {
                for (unsigned int i = 0; i < buttons->size(); i++)
                    (*buttons)[i].layer = -0.2 * i;
            }

            /// GUI output animation
            if (strcmp("fxFlyOut", syntax) == 0) {
                /// set buttons layer for animation
                for (unsigned int j = 0; j < buttons->size(); j++)
                    (*buttons)[(exec + j) % buttons->size()].layer = 1 + 0.2 * j;
                /// cancel executing syntaxes
                busy = true;
                delete[] (*syntaxList)[0];
                syntaxList->erase(syntaxList->begin());
                break;
            }

            /// Get configuration value
            if (strcmp("getCfg", syntax) == 0) {
                int value;
                sscanf((*syntaxList)[0], "%s %d", &syntax[0], &value);
                variable = config[value];
            }

            /// conditional jump if variable is bigger than attribut
            if (strcmp("ifBgr", syntax) == 0) {
                int value;
                char newsyntax[16];
                sscanf((*syntaxList)[0], "%s %d %s", &syntax[0], &value, &newsyntax[0]);
                if (variable > value) {
                    (*syntaxList)[0][0] = '\0';
                    sprintf((*syntaxList)[0], "include %s", &newsyntax[0]);
                    syntax[0] = '\0';
                    strcpy(syntax, "include");
                }
            }

            /// conditional jump if variable is equal than attribut
            if (strcmp("ifEql", syntax) == 0) {
                int value;
                char newsyntax[16];
                sscanf((*syntaxList)[0], "%s %d %s", &syntax[0], &value, &newsyntax[0]);
                if (variable == value) {
                    (*syntaxList)[0][0] = '\0';
                    sprintf((*syntaxList)[0], "include %s", &newsyntax[0]);
                    syntax[0] = '\0';
                    strcpy(syntax, "include");
                }
            }

            /// conditional jump if variable is smaller than attribut
            if (strcmp("ifSml", syntax) == 0) {
                int value;
                char newsyntax[16];
                sscanf((*syntaxList)[0], "%s %d %s", &syntax[0], &value, &newsyntax[0]);
                if (variable < value) {
                    (*syntaxList)[0][0] = '\0';
                    sprintf((*syntaxList)[0], "include %s", &newsyntax[0]);
                    syntax[0] = '\0';
                    strcpy(syntax, "include");
                }
            }

            /// include another script
            if (strcmp("include", syntax) == 0) {
                char value[32];
                sscanf((*syntaxList)[0], "%s %s", &syntax[0], &value[0]);
                delete[] (*syntaxList)[0];
                syntaxList->erase(syntaxList->begin());
                std::vector<char*> *newList = getList(value);
                for (unsigned int i = 0; i < syntaxList->size(); i++) {
                    newList->push_back((*syntaxList)[i]);
                }
                syntaxList->clear();
                delete syntaxList;
                syntaxList = newList;
                break;
            }

            /// load sounds
            if (strcmp("initSound", syntax) == 0) {
                crash = getSound(soundCrash, false, 8);
                engine = getSound(soundEngine, true, 8);
                noise = getSound("sfx/n2o.ogg", true, 8);
            }

            /// load car
            if (strcmp("loadCar", syntax) == 0) {
                float rotation = 0;
                if (allCar[0] != 0) {
                    rotation = allCar[0]->rot;
                }
                for (int i = 0; i < carCount; i++) {
                    delete allCar[i];
                }
                carCount = 0;
                playerCar = variable;
                allCar[0] = new car(0, new std::vector<edge>(), (*carList)[variable], false);
                allCar[0]->rot = rotation;
            }

            /// load game scene
            if (strcmp("loadScene", syntax) == 0) {
                delete physic;
                currentTrack = variable;
#ifdef ZIP_ARCHIVE
                std::vector<char*> *atributes = getFullList(zip_fopen(APKArchive, prefix((*trackList)[currentTrack]), 0));
#else
                std::vector<char*> *atributes = getFullList(fopen(prefix((*trackList)[currentTrack]), "r"));
#endif
                loadScene(atributes);

                /// create instance of physical engine
                physic = getPhysics(trackdata);
                if (trackdata2 != 0) {
                    physic->addModel(trackdata2);
                }
                for (int i = 0; i <= opponentCount; i++)
                    physic->addCar(allCar[i]);
            }

            /// physics locking
            if (strcmp("locked", syntax) == 0) {
                int value;
                sscanf((*syntaxList)[0], "%s %d", &syntax[0], &value);
                if (value == 1)
                    physic->locked = true;
                else
                    physic->locked = false;
            }

            /// decrease variable by value
            if (strcmp("minus", syntax) == 0) {
                int value;
                sscanf((*syntaxList)[0], "%s %d", &syntax[0], &value);
                variable -= value;
            }

            /// decrease variable by value of config
            if (strcmp("minusCfg", syntax) == 0) {
                int value;
                sscanf((*syntaxList)[0], "%s %d", &syntax[0], &value);
                variable -= config[value];
            }

            /// decrease variable by list size
            if (strcmp("minusListSize", syntax) == 0) {
                char value[32];
                sscanf((*syntaxList)[0], "%s %s", &syntax[0], &value[0]);
                variable -= getList(value)->size();
            }

            /// play music
            if (strcmp("musicPlay", syntax) == 0) {
                music = getSound((*musicList)[variable], true, 1);
                music->play(0);
            }

            /// set music volume
            if (strcmp("musicSetVolume", syntax) == 0) {
                music->setVolume(0, variable / 10.0);
            }

            /// stop music
            if (strcmp("musicStop", syntax) == 0) {
                music->stop(0);
            }

            /// mute all sounds
            if (strcmp("muteSound", syntax) == 0) {
                for (int i = 0; i < carCount; i++) {
                    engine->stop(i);
                    noise->stop(i);
                }
            }

            /// set GPS navigation edge by index
            if (strcmp("nav", syntax) == 0) {
                int value;
                sscanf((*syntaxList)[0], "%s %d", &syntax[0], &value);
                allCar[cameraCar]->currentEdge = allCar[cameraCar]->edges[value];
                allCar[cameraCar]->currentGoalEdge = allCar[cameraCar]->edges[value];
                allCar[cameraCar]->finishEdge = value;
                allCar[cameraCar]->lapsToGo = -1;
            }

            /// increase variable by value
            if (strcmp("plus", syntax) == 0) {
                int value;
                sscanf((*syntaxList)[0], "%s %d", &syntax[0], &value);
                variable += value;
            }

            /// set used physical engine
            if (strcmp("physicalEngine", syntax) == 0) {
                sscanf((*syntaxList)[0], "%s %s", &syntax[0], &physicalEngine[0]);
            }

            /// enable race mode
            if (strcmp("race", syntax) == 0) {
                int value;
                sscanf((*syntaxList)[0], "%s %d", &syntax[0], &value);
                if (value == 1)
                    race = true;
                else
                    race = false;
            }

            /// reset car
            if (strcmp("reset", syntax) == 0) {
                if (allCar[cameraCar]->resetAllowed) {
                    if ((allCar[cameraCar]->speed < 5) && active && !physic->locked) {
                        allCar[cameraCar]->resetRequested = true;
                    }
                }
            }

            /// set variable to random number
            if (strcmp("rnd", syntax) == 0) {
                variable = time(NULL) % variable;
            }

            /// quit game
            if (strcmp("quit", syntax) == 0) {
                exit(0);
            }

            /// save all configs
            if (strcmp("saveCfg", syntax) == 0) {
                FILE* file = fopen(prefixEx("config"), "w");
                for (int i = 0; i < configSize; i++) {
                    fprintf(file,"%d\n", config[i]);
                }
                fclose(file);
            }

            /// set variable to value
            if (strcmp("set", syntax) == 0) {
                sscanf((*syntaxList)[0], "%s %d", &syntax[0], &variable);
            }

            /// set config value
            if (strcmp("setCfg", syntax) == 0) {
                int value;
                sscanf((*syntaxList)[0], "%s %d", &syntax[0], &value);
                config[value] = variable;
            }

            /// set GUI background
            if (strcmp("setBackground", syntax) == 0) {
                sscanf((*syntaxList)[0], "%s %d", &syntax[0], &background);
            }

            /// set sound volume
            if (strcmp("setVolume", syntax) == 0) {
                soundVolume = variable;
            }

            /// sleep few seconds
            if (strcmp("sleep", syntax) == 0) {
                int value;
                sscanf((*syntaxList)[0], "%s %d", &syntax[0], &value);
                usleep(value * 1000);
                delete[] (*syntaxList)[0];
                syntaxList->erase(syntaxList->begin());
                break;
            }

            /// multiply variable
            if (strcmp("sub", syntax) == 0) {
                float value;
                sscanf((*syntaxList)[0], "%s %f", &syntax[0], &value);
                variable = (int)(variable * value);
            }

            /// timeout listener
            if (strcmp("timeout", syntax) == 0) {
                sscanf((*syntaxList)[0], "%s %d %s", &syntax[0], &timeout, &timeoutAction[0]);
            }

            /// set transmission type
            if (strcmp("transmission", syntax) == 0) {
                if (variable == 1)
                    transmission = true;
                else
                    transmission = false;
            }

            /// view car in GUI
            if (strcmp("viewCar", syntax) == 0) {
                int value;
                sscanf((*syntaxList)[0], "%s %d", &syntax[0], &value);
                if (value == 1)
                    viewCar = true;
                else
                    viewCar = false;
            }

            /// wait few seconds
            if (strcmp("wait", syntax) == 0) {
                int value;
                sscanf((*syntaxList)[0], "%s %d", &syntax[0], &value);
                unlock = time(NULL) + value;
                delete[] (*syntaxList)[0];
                syntaxList->erase(syntaxList->begin());
                break;
            }

            delete[] (*syntaxList)[0];
            syntaxList->erase(syntaxList->begin());
        }
    }
}
