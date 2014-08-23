//----------------------------------------------------------------------------------------
/**
* \file       io.h
* \author     Vonasek Lubos
* \date       2014/01/05
* \brief      Common input/output utils used in program.
*/
//----------------------------------------------------------------------------------------

#ifndef IO_H
#define IO_H

#include <cstring>
#include <stdio.h>
#include <string>
#include <vector>
#include "common.h"

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

#ifdef ZIP_ARCHIVE
/**
 * @brief gets custom implementation of syntax fgets
 * @param line is data to read
 * @param file is input stream
 */
void gets(char* line, zip_file* file);
#endif

/**
 * @brief gets custom implementation of syntax fgets
 * @param line is data to read
 * @param file is input stream
 */
void gets(char* line, FILE* file);

/**
 * @brief getTag gets indexed tag
 * @param index is index of tag
 * @param text is tag text with %d
 * @return indexed tag
 */
std::string getTag(int index, std::string text);

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
 * @brief prefix prefixes file name
 * @param filename is original file name
 * @return prefixed file name
 */
std::string prefix(std::string filename);

/**
 * @brief scandec read number from chars
 * @param line is chars to read
 * @return number as int
 */
int scandec(char* line);

#endif // IO_H
