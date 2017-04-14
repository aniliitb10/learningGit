#pragma once
#include "Helper.h"
#include "myMathUtils.h"
#include <fstream>

//template<typename T>
//void readData(T& target)
//{
//  target = 0;
//  char c;
//  c = getchar();
//  while ((c > digits::digit0) && (c < digits::digit9))
//  {
//    target *= 10;
//    target += (c - digits::digit0)
//      c = getchar();
//  }
//}
//
//enum class digits : char
//{
//  digit0 = '0',
//  digit9 = '9'
//};

enum coordinates : unsigned int
{
  x = 0,
  y,     // = 1,
  lengthCoordinates // = 2
};

enum orientation : unsigned int
{
  phi1 = 0,
  PHI,   // = 1,
  phi2,  // = 2,
  lengthOrientation // = 3
};

enum position : unsigned int
{
  left = 0,
  right,     // = 1
  leftUp,    // = 2
  rightUp,   // = 3
  leftDown,  // = 4
  rightDown, // = 5
  lengthPosition     // = 6
};

namespace utils
{
  /**
   * gets the value of a tag from a string (this string could even be content of a file)
   * @param content_: the source string
   * @param tag_: looking for the value of this tag
   * @param value_: the value of the tag from the content_(source string)
   * */
  static void pasrseFromString(const std::string& content_, const std::string& tag_, std::string& value_)
  {
    std::regex reg_(".*?" + tag_ + "([^0-9.]*)([0-9.]*)");
    std::smatch matchContent;

    if (std::regex_search(content_, matchContent, reg_))
    {
      value_ = matchContent[2].str();//careful with the indices here
    }
    else
    {
      std::cerr << "ERROR: Couldn't find " << tag_ << " in:\n" << content_ << NEWLINE_CHAR;
      value_ = EMPTY_STR;
    }
  }

  /**
   * gets the entire content of a file in the form of std::string
   * */
  static void getContentFromFile(const std::string& fileName_, std::string& content_)
  {
    std::ifstream file(fileName_);
    if (file.good())
    {
      file.seekg(ZERO, file.end);
      size_t fileLength = static_cast<size_t>(file.tellg());
      content_.resize(fileLength);
      file.seekg(ZERO); //important to reset to start here
      file.read(&content_[ZERO], fileLength);
    }
    else
    {
      content_ = EMPTY_STR;
      std::cerr << "ERROR: Couldn't find the file " << fileName_ << NEWLINE_CHAR;
    }
  }

  /**
   * gets the values of tags from a file (values in std::string)
   * @param fileName_: name of the file
   * @param tagToValuesMap_: a map to store the values of tags
   * */
  static void getTagsFromConfig(const std::string& fileName_, strToStrMap& tagToValuesMap_)
  {
    std::string content;
    getContentFromFile(fileName_, content);
    
    if (!content.empty())
    {
      for (auto& elem : tagToValuesMap_)
      {
        pasrseFromString(content, elem.first, elem.second);
      }
    }
    else
    {
      tagToValuesMap_.clear();
      std::cerr << "ERROR: Couldn't read the config" << NEWLINE_CHAR;
    }
  }
}
