///----------------------------------------------------------------------------------------
/**
 * \file       extfile.cpp
 * \author     Vonasek Lubos
 * \date       2014/12/31
 * \brief      Driver for external file access
**/
///----------------------------------------------------------------------------------------


#include "files/extfile.h"

FILE* extf;

extfile::extfile(std::string filename)
{
  name = filename;
  f = fopen(filename.c_str(), "r");
}

extfile::~extfile()
{
  fclose(f);
}

void extfile::accessStatic()
{
  extf = f;
}

/**
 * @brief gets custom implementation of syntax fgets
 * @param line is data to read
 */
void extfile::gets(char* line)
{
  char character[2];
  for (int i = 0; i < 1020; i++)
  {
    fread(character, 1, 1, f);
    line[i] = character[0];
    if ((line[i] == 10) || (line[i] == 13))
    {
      line[i] = '\n';
      line[i + 1] = '\000';
      return;
    }
  }
  int i = 1020;
  line[i] = '\n';
  line[i + 1] = '\000';
}

/**
* @brief getsEx gets one kilobyte from file archive
* @param line is item to read
*/
void extfile::getsEx(char* line)
{
  line[1023] = '0';
  char character[2];
  for (int i = 0; i < 1020; i++)
  {
    int ok = fread(character, 1, 1, f);
    if (ok <= 0)
    {
      line[i] = '\n';
      line[i + 1] = '\000';
      if (i == 0)
        line[1023] = '1';
      return;
    }
    line[i] = character[0];
    if (line[i] == '\n')
    {
      line[i + 1] = '\000';
      return;
    }
  }
}

/**
 * @brief path gets path of filename
 * @return path as string
 */
std::string extfile::path()
{
  int index = 0;
  for (unsigned int i = 0; i < name.length(); i++)
    if (name[i] == '/')
      index = i;
  return name.substr(0, index + 1);
}

/**
 * @brief scandec read number from file
 * @return number as int
 */
int extfile::scandec()
{
    char line[1024];
    gets(line);
    int number = 0;
    for (int i = 0; i < 1024; i++) {
        if (line[i] != 10)
            number = number * 10 + line[i] - '0';
        else
            return number;
    }
    return number;
}

void png_read_ext(png_structp png_ptr, png_bytep data, png_size_t length)
{
  fread(data, length, 1, extf);
}
