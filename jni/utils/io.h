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

#include "stdafx.h"

/**
 * @brief exists checks if file exist
 * @param filename is name of file to check
 * @return true if file exists
 */
bool exists(const char * filename);

/**
 * @brief getConfig gets config value from file
 * @param item is item to read
 * @param source is source storage
 * @return value in float
 */
float getConfig(const char* item, std::vector<char*> *source);

/**
 * @brief getConfigStr gets config value from file
 * @param item is item to read
 * @param source is source storage
 * @return value in char*
 */
char* getConfigStr(const char* item, std::vector<char*> *source);

/**
 * @brief getExtension gets file extension of file
 * @param filename is filename to get extension
 * @return extension as char*
 */
char* getExtension(const char* filename);

/**
 * @brief getFullList gets list of string from script file
 * @param file is instance of file
 * @return list as vector of char*
 */
#ifdef ZIP_ARCHIVE
std::vector<char*>* getFullList(zip_file* file);
#else
std::vector<char*>* getFullList(FILE* file);
#endif

/**
 * @brief getList gets list of string from script file
 * @param tag is tag of script part
 * @return list as vector of char*
 */
std::vector<char*>* getList(const char* tag);

/**
 * @brief getListEx gets list of string from script file
 * @param tag is tag of script part
 * @param filename is name of file
 * @return list as vector of char*
 */
std::vector<char*>* getListEx(const char* tag, const char* filename);

/**
 * @brief getTag gets indexed tag
 * @param index is index of tag
 * @param text is tag text with %d
 * @return indexed tag
 */
char* getTag(int index, const char* text);

/**
 * @brief loge logs an error
 * @param value1 is a first value
 * @param value2 is a second value
 */
void loge(const char* value1, const char* value2);

/**
 * @brief logi logs an information
 * @param value1 is a first value
 * @param value2 is a second value
 */
void logi(const char* value1, const char* value2);

/**
 * @brief prefix prefixes file name
 * @param filename is original file name
 * @return prefixed file name
 */
char* prefix(const char* filename);

/**
* @brief prefix prefixes file name with config path
* @param filename is original file name
* @return prefixed file name
*/
char* prefixEx(const char* filename);

#endif // IO_H
