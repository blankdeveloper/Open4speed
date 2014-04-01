//----------------------------------------------------------------------------------------
/**
* \file       io.cpp
* \author     Vonasek Lubos
* \date       2014/01/05
* \brief      Common input/output utils used in program.
*/
//----------------------------------------------------------------------------------------

#include <cstdlib>
#include "utils/io.h"
#include "common.h"

char* string1 = new char[255];
char* character = new char[2];  ///< temp char

#ifdef ZIP_ARCHIVE
#ifdef ANDROID
const char* configPath = "/data/data/com.lvonasek.o4s/";    ///< Path to game config
const char* gamePath = "assets/";                           ///< Data path
#else
const char* configPath = "./";                              ///< Path to game config
const char* gamePath = "./";                                ///< Data path
#endif

/**
* @brief getsEx gets config value from zip archive
* @param line is item to read
* @param zip_file is source storage
* @return value in char*
*/
char* getsEx(char* line, zip_file* file) {
    line[1023] = '0';
    for (int i = 0; i < 1020; i++) {
        int ok = zip_fread(file, character, 1);
        if (ok <= 0) {
            line[i] = '\n';
            line[i + 1] = '\000';
            if (i == 0) {
                line[1023] = '1';
            }
            return line;
        }
        line[i] = character[0];
        if (line[i] == '\n') {
            line[i + 1] = '\000';
            return line;
        }
    }
    return line;
}
#else
const char* configPath = "./";          ///< Path to game config
const char* gamePath = "../assets/";    ///< Data path
#endif

#ifdef ANDROID
#include <android/log.h>
#define STRINGIFY(x) #x
#define LOG_TAG    __FILE__ ":" STRINGIFY(__LINE__)
#endif

std::vector<char*> imports = *(new std::vector<char*>());     ///< Config data list

/**
* @brief exists checks if file exist
* @param filename is name of file to check
* @return true if file exists
*/
bool exists(const char * filename) {
   if (FILE* file = fopen(filename, "r")) {
       fclose(file);
       return true;
   }
   return false;
}

/**
* @brief getConfig gets config value from file
* @param item is item to read
* @param source is source storage
* @return value in float
*/
float getConfig(const char* item, std::vector<char*> *source) {
   for (unsigned int i = 0; i < source->size(); i++) {
       float value;
       char name[256];
       sscanf((*source)[i],"%s %f", &name[0], &value);
       if (strcmp(name,item) == 0)
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
char* getConfigStr(const char* item, std::vector<char*> *source) {
   for (unsigned int i = 0; i < source->size(); i++) {
       char name[256];
       char* value = new char[256];
       sscanf((*source)[i],"%s %s", &name[0], value);
       if (strcmp(name,item) == 0) {
           return value;
       }
   }
   char* output = new char[1];
   output[0] = '\0';
   return output;
}

/**
* @brief getExtension gets file extension of file
* @param filename is filename to get extension
* @return extension as char*
*/
char* getExtension(const char* filename) {
   for (int i = strlen(filename) - 1; i > 0; i--) {
       if (filename[i] == '.') {
           for (unsigned int j = i + 1; j < strlen(filename); j++) {
               string[j - i - 1] = filename[j];
           }
           string[strlen(filename) - i - 1] = '\0';
           return string;
       }
   }
   string[0] = '\0';
   return string;
}

/**
* @brief getFullList gets list of string from script file
* @param file is instance of file
* @return list as vector of char*
*/
#ifdef ZIP_ARCHIVE
std::vector<char*>* getFullList(zip_file* file) {
#else
std::vector<char*>* getFullList(FILE* file) {
#endif
   std::vector<char*> *output = new std::vector<char*>();
   while(true) {
       char* value = new char[1024];
#ifdef ZIP_ARCHIVE
       value = getsEx(value, file);
       if (value[1023] == '1') {
               zip_fclose(file);
#else

       /// end of file
       if (fgets(value, 1024, file) == 0) {
               fclose(file);
#endif
               delete[] value;
               return output;
       }

       /// load data
       if (value[0] != '/') {
           if (strlen(value) > 1) {
               while((value[strlen(value) - 1] == '\n') || (value[strlen(value) - 1] == '\r')) {
                   value[strlen(value) - 1] = '\0';
                   if (strlen(value) <= 1) {
                       break;
                   }
               }
           } else {
               value[0] = '\0';
           }
           if (strcmp(value,"END") == 0){
#ifdef ZIP_ARCHIVE
               zip_fclose(file);
#else
               fclose(file);
#endif
               delete[] value;
               return output;
           }
           output->push_back(value);
       }
   }
}

/**
* @brief getList gets list of string from script file
* @param tag is tag of script part
* @return list as vector of char*
*/
std::vector<char*>* getList(const char* tag) {
    for (unsigned int i = 0; i < imports.size(); i++) {
        delete[] imports[i];
    }
    imports.clear();
    return getListEx(tag, "config/open4speed.txt");
}

/**
* @brief getListEx gets list of string from script file
* @param tag is tag of script part
* @param filename is name of file
* @return list as vector of char*
*/
std::vector<char*>* getListEx(const char* tag, const char* filename) {
#ifdef ZIP_ARCHIVE
    zip_file* file = zip_fopen(APKArchive, prefix(filename), 0);
#else
    FILE* file = fopen(prefix(filename), "r");
#endif
    while(true) {
       char value[1024];
#ifdef ZIP_ARCHIVE
       getsEx(value, file);
       if (value[1023] == '1') {
           if (imports.size() > 0) {
               zip_fclose(file);
               file = zip_fopen(APKArchive, prefix(imports[0]), 0);
#else
       fgets(value, 1024, file);

       /// end of file
       if (feof(file)) {
           if (imports.size() > 0) {
               fclose(file);
               file = fopen(prefix(imports[0]), "r");

#endif
               filename = imports[0];
               delete[] imports[0];
               imports.erase(imports.begin());
               continue;
           } else {
#ifdef ZIP_ARCHIVE
               zip_fclose(file);
#else
               fclose(file);
#endif
               return new std::vector<char*>();
           }
       }

       /// fix end of line
       else if (value[strlen(value) - 1] == '\n') {
           value[strlen(value) - 1] = '\0';
       }

       /// begin of script
       if (strcmp(value,tag) == 0){
           break;
       }

       /// import other file
       else if (value[0] == '#') {
           /// import file
           char* name = new char[1024];
           sscanf(value,"#import %s", name);
           imports.push_back(name);
       }
   }
   return getFullList(file);
}

#ifdef ZIP_ARCHIVE
/**
 * @brief gets custom implementation of syntax fgets
 * @param line is data to read
 * @param file is input stream
 */
void gets(char* line, zip_file* file) {
    for (int i = 0; i < 1020; i++) {
        zip_fread(file, character, 1);
        line[i] = character[0];
        if (line[i] == '\n') {
            line[i + 1] = '\000';
            return;
        }
    }
}
#endif

/**
 * @brief gets custom implementation of syntax fgets
 * @param line is data to read
 * @param file is input stream
 */
void gets(char* line, FILE* file) {
    for (int i = 0; i < 1020; i++) {
        fread(character, 1, 1, file);
        line[i] = character[0];
        if ((line[i] == 10) || (line[i] == 13)) {
            line[i] = '\n';
            line[i + 1] = '\000';
            return;
        }
    }
}

/**
* @brief getTag gets indexed tag
* @param index is index of tag
* @param text is tag text with %d
* @return indexed tag
*/
char* getTag(int index, const char* text) {
   sprintf(string1, text, index);
   return string1;
}

/**
* @brief loge logs an error
* @param value1 is a first value
* @param value2 is a second value
*/
void loge(const char* value1, const char* value2) {
 printf("%s %s\n", value1, value2);
#ifdef ANDROID
 __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,"com.lvonasek.o4s:%s %s", value1, value2);
#endif
 exit(1);
}

/**
* @brief logi logs an information
* @param value1 is a first value
* @param value2 is a second value
*/
void logi(const char* value1, const char* value2) {
 printf("%s %s\n", value1, value2);
#ifdef ANDROID
 __android_log_print(ANDROID_LOG_INFO,LOG_TAG,"com.lvonasek.o4s:%s %s", value1, value2);
#endif
}

/**
* @brief prefix prefixes file name
* @param filename is original file name
* @return prefixed file name
*/
char* prefix(const char* filename) {
   strcpy(string, gamePath);
   strcat(string, filename);
   return string;
}

/**
* @brief prefix prefixes file name with config path
* @param filename is original file name
* @return prefixed file name
*/
char* prefixEx(const char* filename) {
    strcpy(string, configPath);
    strcat(string, filename);
    return string;
}

/**
 * @brief scandec read number from chars
 * @param line is chars to read
 * @return number as int
 */
int scandec(char* line) {
    int number = 0;
    for (int i = 0; i < 1024; i++) {
        if (line[i] != 10) {
            number = number * 10 + line[i] - '0';
        } else {
            return number;
        }
    }
    return number;
}
