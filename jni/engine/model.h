///----------------------------------------------------------------------------------------
/**
 * \file       model.h
 * \author     Vonasek Lubos
 * \date       2014/12/30
 * \brief      Loading and storing models
**/
///----------------------------------------------------------------------------------------

#ifndef MODEL_H
#define MODEL_H

#include <map>
#include <string>
#include "engine/math.h"
#include "interfaces/materialLoader.h"

#define CULLING_DST 100

struct id3d
{
    int x;
    int y;
    int z;

    bool operator==(id3d v)
    {
        return (v.x == x) && (v.y == y) && (v.z == z);
    }

    bool operator!=(id3d v)
    {
        return !(*this==v);
    }
};

bool operator<(const id3d& lhs, const id3d& rhs);

/**
 * @brief The model3d struct
 */
struct model3d
{
    bool touchable;              ///< info if it is used in physics
    int filter;                  ///< filter index
    shader* material;            ///< shader to use
    bool dynamic;                ///< True if object is dynamic
    int dynamicID;               ///< ID of the dynamic object
    float dynamicMat[16];        ///< Model matrix of dynamic object
    AABB reg;                    ///< AABB of the object
    texture* texture2D;          ///< Object texture
    std::vector<float> vertices; ///< Object vertices
    std::vector<float> normals;  ///< Object normals
    std::vector<float> coords;   ///< Object texture coordinates
};

/**
 * @brief The model class
 */
class model
{
public:
    /**
     * @brief model destructor
     */
    ~model();

    /**
     * @brief Constructor for loading model from file
     * @param filename is path and name of file to load
     * @param mtlLoader is instance of object for loading materials
     */
    model(std::string filename, materialLoader* mtlLoader);

    std::vector<model3d> models;               ///< Standard parts of model
    AABB aabb;                                 ///< Extremes of current model
    bool toDelete;                             ///< Additional information for culling
};

#endif // MODEL_H
