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

#include <glm/glm.hpp>
#include <map>
#include <string>
#include "engine/math.h"
#include "interfaces/shader.h"
#include "interfaces/texture.h"

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
struct model3d {
    bool touchable;     ///< info if it is used in physics
    int filter;         ///< filter index
    shader* material;   ///< shader to use
    float colora[4];    ///< Ambient color
    float colord[4];    ///< Diffuse color
    float colors[4];    ///< Specular color
    bool dynamic;       ///< True if object is dynamic
    int dynamicID;      ///< ID of the last dynamic update
    bool hasShadow;     ///< Information that model casts shadow
    AABB reg;           ///< AABB of the object
    int count;          ///< Amount of triangles
    texture* texture2D; ///< Object texture
    float* vertices;    ///< Object vertices
    float* normals;     ///< Object normals
    float* coords;      ///< Object texture coordinates
    float x,y,z;        ///< Object translation
};

/**
 * @brief The model interface
 */
class model {
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

    void triangles(model3d* m, bool culling);

    void detexturise(bool culling);

    std::vector<model3d> models;               ///< Standard parts of model
    AABB aabb;                                 ///< Extremes of current model
    float width, aplitude, height;             ///< Dimensions of current model
    char modelname[256];                       ///< Model filename

    std::map<id3d, std::vector<float> > vertices;
    std::map<id3d, std::vector<float> > colors;
    bool culled;
};

#endif // MODEL_H
