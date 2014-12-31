///----------------------------------------------------------------------------------------
/**
 * \file       io.cpp
 * \author     Vonasek Lubos
 * \date       2014/12/31
 * \brief      Common input/output utils used in program.
**/
///----------------------------------------------------------------------------------------

#include "engine/io.h"

#ifdef ANDROID
#include <android/log.h>
#define STRINGIFY(x) #x
#define LOG_TAG    __FILE__ ":" STRINGIFY(__LINE__)
#endif

std::vector<std::string> imports;     ///< Config data list


std::string fixName(std::string filename) {
    int index = 0;
    for (unsigned int i = 1; i < filename.size(); i++)
      if (filename[i] == '#')
          index = i;
    filename = filename.substr(index, filename.size() - index);
    while(true) {
      index = 0;
      for (unsigned int i = 1; i < filename.size(); i++) {
        if ((filename[i] == '/') && (filename[i + 1] != '.'))
          index = i;
        else if ((filename[i - 1] == '/') && (filename[i] == '.')) {
          filename = filename.substr(0, index) + filename.substr(i + 2, filename.size() - i);
          return filename;
        }
      }
      return filename;
    }
}

/**
* @brief getConfig gets config value from file
* @param item is item to read
* @param source is source storage
* @return value in float
*/
float getConfig(std::string item, std::vector<std::string> source) {
   for (unsigned int i = 0; i < source.size(); i++) {
       float value;
       char name[256];
       sscanf(source[i].c_str(), "%s %f", &name[0], &value);
       if (strcmp(name,item.c_str()) == 0)
           return value;
   }
   return 0;
}

/**
* @brief getConfigStr gets config value from file
* @param item is item to read
* @param source is source storage
* @return value in char*
*/
std::string getConfigStr(std::string item, std::vector<std::string> source) {
   for (unsigned int i = 0; i < source.size(); i++) {
       char name[256];
       char value[256];
       sscanf(source[i].c_str(), "%s %s", &name[0], value);
       if (strcmp(name,item.c_str()) == 0) {
           return std::string(value);
       }
   }
   return std::string();
}

/**
* @brief getExtension gets file extension of file
* @param filename is filename to get extension
* @return extension as char*
*/
std::string getExtension(std::string filename) {
   char string[1024];
   for (int i = filename.length() - 1; i > 0; i--) {
       if (filename[i] == '.') {
           for (unsigned int j = i + 1; j < filename.length(); j++) {
               string[j - i - 1] = filename[j];
           }
           string[filename.length() - i - 1] = '\0';
           return string;
       }
   }
   return std::string();
}

/**
* @brief getList gets list of string from script file
* @param tag is tag of script part
* @param filename is name of file
* @return list as vector of char*
*/
std::vector<std::string> getList(std::string tag, std::string filename) {
    file* f = getFile(filename);
    std::vector<std::string> output;
    bool hasTag = false;
    if (tag.length() == 0)
        hasTag = true;
    while(true) {
       char value[1024];
       strcpy(value, "");
       f->getsEx(value);

       /// fix end of line
       if (strlen(value) > 0)
           if (value[strlen(value) - 1] == '\n')
               value[strlen(value) - 1] = '\0';

       /// begin of script
       if (strcmp(value,tag.c_str()) == 0){
           hasTag = true;
       }

       /// import other file
       else if (value[0] == '#') {
           /// import file
           char name[1024];
           sscanf(value,"#include %s", name);
           imports.push_back(std::string(name));
       }

       /// load data
       else if (value[0] != '/') {
           if (strlen(value) > 1) {
               while((value[strlen(value) - 1] == '\n') || (value[strlen(value) - 1] == '\r')) {
                   value[strlen(value) - 1] = '\0';
                   if (strlen(value) <= 1) {
                       break;
                   }
               }
           }
           if (hasTag && strcmp(value,"END") == 0){
               delete f;
               return output;
           }
           if (hasTag)
             output.push_back(value);
       }

       /// end of file
       if (value[1023] == '1') {
           if (imports.size() > 0) {
               delete f;
               f = getFile(imports[0]);
               imports.clear();
               continue;
           } else {
               delete f;
               return output;
           }
       }
   }
}

/**
* @brief loge logs an error
* @param value1 is a first value
* @param value2 is a second value
*/
void loge(std::string value1, std::string value2) {
 printf("%s %s\n", value1.c_str(), value2.c_str());
#ifdef ANDROID
 __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,"com.lvonasek.o4s:%s %s", value1.c_str(), value2.c_str());
#endif
}

/**
* @brief logi logs an information
* @param value1 is a first value
* @param value2 is a second value
*/
void logi(std::string value1, std::string value2) {
 printf("%s %s\n", value1.c_str(), value2.c_str());
#ifdef ANDROID
 __android_log_print(ANDROID_LOG_INFO,LOG_TAG,"com.lvonasek.o4s:%s %s", value1.c_str(), value2.c_str());
#endif
}
