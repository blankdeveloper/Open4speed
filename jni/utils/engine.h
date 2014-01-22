//----------------------------------------------------------------------------------------
/**
 * \file       engine.h
 * \author     Vonasek Lubos
 * \date       2014/01/05
 * \brief      Common engine functions used in program.
*/
//----------------------------------------------------------------------------------------

#ifndef ENGINE_H
#define ENGINE_H

#include "stdafx.h"

/**
 * @brief The button struct
 */
struct button {
    int image;          ///< Image index
    int text;           ///< Text index
    int type;           ///< Button type
    float x;            ///< X coordinate
    float y;            ///< Y coordinate
    float width;        ///< Width
    float height;       ///< Height
    char syntax[32];    ///< Syntax called after pressing button
    float layer;        ///< Distance from the camera
};

/**
 * @brief displayMenu displaies menu
 * @param gui is instance of menu
 * @param renderer is instance of renderer
 * @param images is storage of textures
 * @param textList is storage of strings
 */
void displayMenu();

/**
 * @brief displayScene displaies race scene
 * @param renderer is instance of renderer
 * @param gui is instance of menu
 * @param track is instance of track
 * @param physics is instance of physical engine
 */
void displayScene();

/**
 * @brief loadScene loads scene from Open4speed config file
 * @param track is instance of last track
 * @param gui is instance of menu
 * @param atributes is track information
 * @param playerCar  is index of choosen car
 * @return loaded scene
 */
void loadScene(std::vector<char*> *atributes);

/**
 * @brief updateMenu updates menu state
 * @param gui is instance of menu
 * @return updated instance
 */
void updateMenu();


#endif // ENGINE_H
