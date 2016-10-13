///----------------------------------------------------------------------------------------
/**
 * \file       zipfile.h
 * \author     Vonasek Lubos
 * \date       2014/12/31
 * \brief      Driver for zip file access
**/
///----------------------------------------------------------------------------------------

#ifndef ZIPFILE_H
#define ZIPFILE_H

#include <string>
#include <zip.h>
#include "interfaces/file.h"

class zipfile : public file
{
public:
    zipfile(std::string filename, zip* archive);

    ~zipfile();

    void accessStatic();

    /**
     * @brief gets custom implementation of syntax fgets
     * @param line is data to read
     */
    void gets(char* line);

    /**
    * @brief getsEx gets one kilobyte from zip archive
    * @param line is item to read
    */
    void getsEx(char* line);

    bool isArchive() { return true; }

    /**
     * @brief path gets path of filename
     * @return path as string
     */
    std::string path();

    /**
     * @brief scandec read number from file
     * @return number as int
     */
    int scandec();
private:
    zip_file* f;
};

void png_read_zip(png_structp png_ptr, png_bytep data, png_size_t length);

#endif // ZIPFILE_H
