///----------------------------------------------------------------------------------------
/**
 * \file       io.h
 * \author     Vonasek Lubos
 * \date       2014/12/31
 * \brief      Common input/output utils used in program.
**/
///----------------------------------------------------------------------------------------

#ifndef IO_H
#define IO_H

#include <string>
#include <vector>
#include <zip.h>
#include "interfaces/file.h"

std::string fixName(std::string filename);

/**
 * @brief getConfig gets config value from file
 * @param item is item to read
 * @param source is source storage
 * @return value in float
 */
float getConfig(std::string item, std::vector<std::string> source);

/**
 * @brief getConfigStr gets config value from file
 * @param item is item to read
 * @param source is source storage
 * @return value in char*
 */
std::string getConfigStr(std::string item, std::vector<std::string> source);

/**
 * @brief getExtension gets file extension of file
 * @param filename is filename to get extension
 * @return extension as char*
 */
std::string getExtension(std::string filename);

/**
 * @brief getFile returns instance of file reader
 * @param filename is path to file
 * @return instance of file
 */
file* getFile(std::string filename);

/**
 * @brief getListEx gets list of string from script file
 * @param tag is tag of script part
 * @param filename is name of file
 * @return list as vector of char*
 */
std::vector<std::string> getList(std::string tag, std::string filename);

/**
 * @brief loge logs an error
 * @param value1 is a first value
 * @param value2 is a second value
 */
void loge(std::string value1, std::string value2);

/**
 * @brief logi logs an information
 * @param value1 is a first value
 * @param value2 is a second value
 */
void logi(std::string value1, std::string value2);

/**
 * @brief str converts int into string
 * @param i is int number
 * @return number as string
 */
std::string str(int i);

/**
 * @brief setZip sets APK archive object
 * @param path is path of APK
 */
void setZip(std::string path);

#endif // IO_H
