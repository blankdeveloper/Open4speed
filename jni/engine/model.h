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
#include "interfaces/shader.h"
#include "interfaces/texture.h"

#define CULLING_DST 100

struct id3d
{
    int x;
    int y;
    int z;
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
 * @brief The model interface
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
     */
    model(std::string filename);

    void culling();

    std::vector<model3d> models;               ///< Standard parts of model
    std::map<id3d, std::vector<model3d> > v3d; ///< Models with culling
    AABB aabb;                                 ///< Extremes of current model
};

#endif // MODEL_H
