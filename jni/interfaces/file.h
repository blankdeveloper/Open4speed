///----------------------------------------------------------------------------------------
/**
 * \file       file.h
 * \author     Vonasek Lubos
 * \date       2014/12/31
 * \brief      Interface for file access
**/
///----------------------------------------------------------------------------------------

#ifndef FILE_H
#define FILE_H

#include <string>

class file
{
protected:
    std::string name;
public:

    virtual ~file() {}

    virtual void accessStatic() = 0;

    /**
     * @brief gets custom implementation of syntax fgets
     * @param line is data to read
     */
    virtual void gets(char* line) = 0;

    /**
    * @brief getsEx gets config value from zip archive
    * @param line is item to read
    */
    virtual void getsEx(char* line) = 0;

    virtual bool isArchive() = 0;

    /**
     * @brief path gets path of file
     * @return path as string
     */
    virtual std::string path() = 0;

    /**
     * @brief scandec read number from file
     * @return number as int
     */
    virtual int scandec() = 0;
};

#endif // FILE_H
