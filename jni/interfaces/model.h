//----------------------------------------------------------------------------------------
/**
 * \file       model.h
 * \author     Vonasek Lubos
 * \date       2014/02/25
 * \brief      Loading and storing models
*/
//----------------------------------------------------------------------------------------

#ifndef MODEL_H
#define MODEL_H

#include <glm/glm.hpp>
#include "interfaces/fbo.h"
#include "interfaces/shader.h"
#include "interfaces/texture.h"
#include "interfaces/vbo.h"

/**
 * @brief The edge struct
 */
struct edge {
    float ax;    ///< Point coordinate
    float ay;    ///< Point coordinate
    float az;    ///< Point coordinate
    float bx;    ///< Point coordinate
    float by;    ///< Point coordinate
    float bz;    ///< Point coordinate
};

/**
 * @brief AABB struct
 */
struct AABB {
    glm::vec3 min; ///< Region extreme
    glm::vec3 max; ///< Region extreme
};

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
    AABB reg;           ///< AABB of the object
    int* triangleCount; ///< Amount of triangles
    texture* texture2D; ///< Object texture
    float* vertices;    ///< Object vertices
    float* normals;     ///< Object normals
    float* tnormals;    ///< Object triangle normals
    float* coords;      ///< Object texture coordinates
    float x,y,z;        ///< Object translation
    vbo* vboData;       ///< VBO of model
};

/**
 * @brief The model interface
 */
class model {
public:

    /**
     * @brief model destructor
     */
    virtual ~model() = 0;

    int cutX, cutY, edgesCount;                ///< Size of SS Culling
    std::vector<edge> *edges;                  ///< All standalone edges of model
    std::vector<model3d> models;               ///< All parts of model
    AABB aabb;                                 ///< Extremes of current model
    float width, aplitude, height;             ///< Dimensions of current model
};

#endif // MODEL_H
