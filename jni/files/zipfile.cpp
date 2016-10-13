///----------------------------------------------------------------------------------------
/**
 * \file       zipfile.cpp
 * \author     Vonasek Lubos
 * \date       2014/12/31
 * \brief      Driver for zip file access
**/
///----------------------------------------------------------------------------------------

#include "engine/io.h"
#include "files/zipfile.h"

zip_file* zipf;

zipfile::zipfile(std::string filename, zip* archive)
{
  filename = fixName(filename);
  name = filename;
  f = zip_fopen(archive, filename.c_str(), 0);
}

zipfile::~zipfile()
{
  zip_fclose(f);
}

void zipfile::accessStatic()
{
  zipf = f;
}

/**
 * @brief gets custom implementation of syntax fgets
 * @param line is data to read
 */
void zipfile::gets(char* line)
{
  char character[2];
  for (int i = 0; i < 1020; i++)
  {
    zip_fread(f, character, 1);
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
* @brief getsEx gets one kilobyte from zip archive
* @param line is item to read
*/
void zipfile::getsEx(char* line)
{
  line[1023] = '0';
  char character[2];
  for (int i = 0; i < 1020; i++)
  {
    int ok = zip_fread(f, character, 1);
    if (ok <= 0) {
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
std::string zipfile::path()
{
  int index = 0;
  for (unsigned int i = 0; i < name.length(); i++)
    if (name[i] == '/')
      index = i;
  return "#" + name.substr(0, index + 1);
}

/**
 * @brief scandec read number from file
 * @return number as int
 */
int zipfile::scandec()
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

void png_read_zip(png_structp png_ptr, png_bytep data, png_size_t length)
{
  zip_fread(zipf, data, length);
}
