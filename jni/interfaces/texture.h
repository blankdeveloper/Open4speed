///----------------------------------------------------------------------------------------
/**
 * \file       texture.h
 * \author     Vonasek Lubos
 * \date       2014/12/30
 * \brief      Loading and storing textures
**/
///----------------------------------------------------------------------------------------

#ifndef TEXTURE_H
#define TEXTURE_H

#include <vector>

// Struct for color of pixel r,g,b,a red, green, blue, alpha values
struct ColorRGBA {
    unsigned char r,g,b,a;
};

// Struct for color of pixel r,g,b red, green, blue values
struct ColorRGB {
    unsigned char r,g,b;
};

struct Texture {
    int width;
    int height;
    unsigned char* data;
    bool hasAlpha;
};

/**
 * @brief The texture interface
 */
class texture {
public:
    bool transparent;       ///< Texture transparency
    int twidth, theight;    ///< Image dimensions
    char texturename[256];  ///< Texture filename
    unsigned int textureID; ///< Texture id

    bool animated;                  ///< True if it is texture sequence
    std::vector<texture*> anim;     ///< Animation images
    unsigned int currentFrame;      ///< Current image
    unsigned int currentMultiFrame; ///< Replying frames state
    unsigned int multiFrame;        ///< Replying frames amount

    virtual ~texture() {}

    /**
     * @brief apply applies current texture
     */
    virtual void apply() = 0;

    virtual ColorRGBA getPixel(double s, double t) = 0;

    /**
     * @brief setFrame set frame of animation
     * @param frame is index of frame
     */
    virtual void setFrame(int frame) = 0;
};

#endif // TEXTURE_H
