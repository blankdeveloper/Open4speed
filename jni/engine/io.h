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

#include <cstring>
#include <sstream>
#include <string>
#include <vector>
#include "engine/switch.h"

#define SSTR( x ) dynamic_cast< std::ostringstream & >( ( std::ostringstream() << std::dec << x ) ).str()

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

#endif // IO_H
