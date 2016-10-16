///----------------------------------------------------------------------------------------
/**
 * \file       gltexture.cpp
 * \author     Vonasek Lubos
 * \date       2014/12/30
 * \brief      Loading and storing textures in OpenGL
**/
///----------------------------------------------------------------------------------------

#include <stdio.h>
#include "renderers/opengl/gles20.h"
#include "renderers/opengl/gltexture.h"

/**
 * @brief destruct removes texture from memory
 */
gltexture::~gltexture()
{
    if (!animated)
    {
        if (data)
            delete[] data;
        glDeleteTextures(1, &textureID);
    }
    else
    {
        for (int i = anim.size() - 1; i >= 0; i--)
            delete anim[i];
    }
}

/**
 * @brief gltexture creates animated empty texture instance
 * @param anim is an image sequence
 */
gltexture::gltexture(std::vector<texture*> anim)
{
    this->anim = anim;
    transparent = true;
    animated = true;
    instanceCount = 1;

    /// set animation speed
    multiFrame = anim.size() / 50;
    if (anim.size() < 10)
        multiFrame = 3;
    if (multiFrame < 1)
        multiFrame = 1;

    currentFrame = 0;
    currentMultiFrame = 0;
}

/**
 * @brief gltexture creates texture from raster data
 * @param texture is texture raster instance
 * @param alpha is amount of blending
 */
gltexture::gltexture(Texture texture)
{
    instanceCount = 1;
    twidth = texture.width;
    theight = texture.height;
    transparent = texture.hasAlpha;
    animated = false;
    data = texture.data;
    hasAlpha = texture.hasAlpha;
}

/**
 * @brief apply applies current texture
 */
void gltexture::apply()
{
    if (animated)
    {
        anim[currentFrame]->apply();

        /// update animation
        currentMultiFrame++;
        if (currentMultiFrame == multiFrame)
        {
            currentMultiFrame = 0;
            currentFrame++;
        }
        if (currentFrame == anim.size())
            currentFrame = 0;
    } else
    {
        /// upload texture
        if (data)
        {
            glGenTextures(1, &textureID);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, textureID);

            //And if you go and use extensions, you can use Anisotropic filtering textures which are of an
            //even better quality, but this will do for now.
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

            //Here we are setting the parameter to repeat the texture instead of clamping the texture
            //to the edge of our shape.
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
            if (hasAlpha)
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, twidth, theight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            else
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, twidth, theight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            delete[] data;
            data = 0;
        }
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID);
    }
}

/**
 * @brief setFrame set frame of animation
 * @param frame is index of frame
 */
void gltexture::setFrame(int frame)
{
    currentFrame = frame;
}
