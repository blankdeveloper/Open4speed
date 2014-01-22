//----------------------------------------------------------------------------------------
/**
 * \file       rgb.cpp
 * \author     Vonasek Lubos
 * \date       2014/01/05
 * \brief      Loading and storing textures
*/
//----------------------------------------------------------------------------------------

#include "stdafx.h"

/**
 * @brief destruct removes texture from memory is there is no more instance
 */
void rgb::pointerDecrease() {
  instanceCount--;
  if (instanceCount == 0) {
    glDeleteTextures(1, &textureID);
  }
}

/**
 * @brief rgb creates texture from color
 * @param r is red color value
 * @param g is green color value
 * @param b is blue color value
 * @param alpha is amount of blending
 */
rgb::rgb(float r, float g, float b, float alpha) {
    /// define variables for texture
    int sizeX;
    int sizeY;
    char* data;

    /// create color pixel
    sizeX = sizeY = 1;
    data = new char[(int)1 * 4];
    data[0] = (int)(255 * r);
    data[1] = (int)(255 * g);
    data[2] = (int)(255 * b);
    data[3] = (int)(255 * 1);

    /// create texture
    this->alpha = alpha;
    this->transparent = false;
    glGenTextures(1, &this->textureID);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, this->textureID);

    //And if you go and use extensions, you can use Anisotropic filtering textures which are of an
    //even better quality, but this will do for now.
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    //Here we are setting the parameter to repeat the texture instead of clamping the texture
    //to the edge of our shape.
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sizeX, sizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    instanceCount = 1;
    texturename[0] = '\0';
    twidth = 1;
    theight = 1;
}

/**
 * @brief apply applies current texture
 */
void rgb::apply() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);
}

/**
 * @brief setFrame set frame of animation
 * @param frame is index of frame
 */
void rgb::setFrame(int frame) {

}
