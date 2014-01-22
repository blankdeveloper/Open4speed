//----------------------------------------------------------------------------------------
/**
 * \file       bmp24.cpp
 * \author     Vonasek Lubos
 * \date       2014/01/05
 * \brief      Loading and storing textures
*/
//----------------------------------------------------------------------------------------

#include "stdafx.h"

/**
 * @brief destruct removes texture from memory is there is no more instance
 */
void bmp24::pointerDecrease() {
    instanceCount--;
    if (instanceCount == 0) {
      glDeleteTextures(1, &textureID);
      for (unsigned int i = 0; i < textures->size(); i++) {
          if (strcmp(texturename, (*textures)[i]->texturename) == 0) {
              (*textures).erase(textures->begin() + i);
          }
      }
    }
}

/**
 * @brief bmp24 loads texture from bmp24 file
 * @param filename is name of file
 * @param alpha is amount of blending
 */
bmp24::bmp24(const char* filename, float alpha) {
    /// define variables for texture
    this->alpha = alpha;
    int sizeX;
    int sizeY;
    char* data = new char[24];

    /// open file
#ifdef ZIP_ARCHIVE
    zip_file* file = zip_fopen(APKArchive, prefix(filename), 0);
    zip_fread(file, data, 18);
    zip_fread(file, &sizeX, 4);
    zip_fread(file, &sizeY, 4);
#else
    FILE* file = fopen(prefix(filename), "rb");
    fread(data, 18, 1, file);
    fread(&sizeX, 4, 1, file);
    fread(&sizeY, 4, 1, file);
#endif

    /// get image dimensions
    unsigned long size = sizeX * sizeY;
    data = new char[(int)size * 4];

    /// read BMP parameters
    unsigned short int planes;
    unsigned short int bpp;
#ifdef ZIP_ARCHIVE
    zip_fread(file, &planes, 2);
    zip_fread(file, &bpp, 2);
    zip_fread(file, data, 24);
#else
    fread(&planes, 2, 1, file);
    fread(&bpp, 2, 1, file);
    fread(data, 24, 1, file);
#endif

    /// load data
    char* bmp = new char[(int)size * 3];
#ifdef ZIP_ARCHIVE
    zip_fread(file, bmp, (int)size * 3);
#else
    fread(bmp, (int)size * 3, 1, file);
#endif

    /// correct colors (from BGR to RGB)
    transparent = false;
    for (int j=0;j<(int)size;j++) {
        data[j * 4 + 0] = bmp[j * 3 + 2];
        data[j * 4 + 1] = bmp[j * 3 + 1];
        data[j * 4 + 2] = bmp[j * 3 + 0];
        data[j * 4 + 3] = (int)(255 * alpha);
        /// if color is pink then set transparent parameter and change alpha of this color
        if ((data[j * 4 + 0] == (char)255) & (data[j * 4 + 1] == (char)0) & (data[j * 4 + 2] == (char)255)) {
            transparent = true;
            data[j * 4 + 0] = 0;
            data[j * 4 + 1] = 0;
            data[j * 4 + 2] = 0;
            data[j * 4 + 3] = 0;
        }
    }

    /// create texture
    glGenTextures(1, &textureID);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);

    //And if you go and use extensions, you can use Anisotropic filtering textures which are of an
    //even better quality, but this will do for now.
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sizeX, sizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    free(data);

#ifdef ZIP_ARCHIVE
    zip_fclose(file);
#else
    fclose(file);
#endif
}

/**
 * @brief apply applies current texture
 */
void bmp24::apply() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);
}

/**
 * @brief setFrame set frame of animation
 * @param frame is index of frame
 */
void bmp24::setFrame(int frame) {

}
