#pragma once

#include "myMathUtils.h"
#include <fstream>
#include <exception>
#include <string>
#include <cmath>

enum coordinates : size_t
{
  x = 0,
  y,                // = 1,
  lengthCoordinates // = 2
};

enum orientation : size_t
{
  phi1 = 0,
  PHI,              // = 1,
  phi2,             // = 2,
  lengthOrientation // = 3
};

enum position : size_t
{
  left = 0,
  right,          // = 1
  leftUp,         // = 2
  rightUp,        // = 3
  leftDown,       // = 4
  rightDown,      // = 5
  lengthPosition  // = 6
};

enum PointsStructure : size_t
{
  evenNumbered = 0,
  oddNumbered // = 1
};

/**
* implicitly declared copy assignment operator is defined as deleted for Neighbors class
* as it contains non-static data member of non-class type that is const
*/
struct Neighbors 
{
  Neighbors(const size_t& numOfPointsOnXAxis_ = 0) :
    left(1), // needs negation
    right(1),
    leftUp(numOfPointsOnXAxis_ - 1),
    rightUp(numOfPointsOnXAxis_),
    leftDown(numOfPointsOnXAxis_), // needs negation
    rightDown (numOfPointsOnXAxis_ - 1) // needs negation
  {
  }
  const size_t left;
  const size_t right;
  const size_t leftUp;
  const size_t rightUp;
  const size_t leftDown;
  const size_t rightDown;
};


class Custom_Exception : public std::exception
{
public:
  Custom_Exception(std::string msg_):_msg(msg_) {}
  virtual const char* what() const noexcept { return _msg.c_str(); }

private:
  std::string _msg;
};

#define CUSTOM_ASSERT(condition, msg) do { if (!(condition)) throw Custom_Exception(msg); } while(0)

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
      file.seekg(ZERO_INT, file.end);
      size_t fileLength = static_cast<size_t>(file.tellg());
      content_.resize(fileLength);
      file.seekg(ZERO_INT); //important to reset to start here
      file.read(&content_[ZERO_INT], fileLength);
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

  /**
  * finds the root of an index from id
  * @param id_: the vector of all ids
  * @param index_: the index whose id to look for
  * @return: id of the index_ from id_
  * */
  static size_t findRoot(const size_tVec& id_, size_t index_)
  {
    while (index_ != id_[index_])
      index_ = id_[id_[index_]];     //path compression;
    return index_;
  }

  /**
  * joins the two points, precisely, makes their ids same
  * it is weighted, efficient. Looks at the size of points before making their ids same
  * so that, one doesn't become too long
  * @param id_: the vector of all ids
  * @param size_: the vector of size of all ids
  * @param: index1_: index of one of the points to join
  * @param: index2_: index of the other point to join
  * */
  static void unionPoints(size_tVec& id_, size_tVec& size_, size_t index1_, size_t index2_)
  {
    size_t l = findRoot(id_, index1_);
    size_t m = findRoot(id_, index2_);
    size_t size_l = size_[l];
    size_t size_m = size_[m];

    if (l != m)
    {
      if (size_l < size_m)   //weighted! efficient!
      {
        id_[l] = m;
        size_[m] += size_l;
      }
      else
      {
        id_[m] = l;
        size_[l] += size_m;
      }
    }
  }

  static void binnyfy(const doubleVec& values_, size_tVec& yAxis_, doubleVec& xAxis_)
  {

    const double max = *(std::max_element(values_.begin(), values_.end()));
    const double min = *(std::min_element(values_.begin(), values_.end()));
    const size_t bins = yAxis_.size();

    const double xRange = (max - min) / static_cast<double>(bins); //usually this means (max_-0)/bins;
    const double l = xRange / 2.0;
    xAxis_[0] = min + l;
    for (int i = 1; i < bins; ++i)
      xAxis_[i] = xAxis_[i - 1] + xRange;

    double values_size = values_.size();

    for (int i = 0; i < values_size; ++i)
    {
      for (int j = 0; j < bins; ++j)
      {
        if (values_[i] >= (xAxis_[j] - l) && values_[i] < (xAxis_[j] + l))
        {
          yAxis_[j] += 1;
          break;
        }
      }
    }

    for (unsigned int i = 0; i < values_size; ++i)
    {
      if (values_[i] < ((xAxis_[0] - l)))
        yAxis_[0] += 1;
      else if (values_[i] > (((xAxis_[bins - 1] + l))))
        yAxis_[bins - 1] += 1;
    }
  }

  static void writeDataToFile(const std::string& fileName_, const size_tVec& yAxis_, const doubleVec& xAxis_)
  {
    std::ofstream fileHandler(fileName_, std::ios::out | std::ios::trunc);
    if (!fileHandler.good())
    {
      throw Custom_Exception("Couldn't open the file: " + fileName_);
    }

    CUSTOM_ASSERT(yAxis_.size() == xAxis_.size(), "Number of elements in x-axis and y-axis aren't same");

    for (size_t index = 0; index < xAxis_.size(); ++index)
    {
      fileHandler << xAxis_[index] << "\t" << yAxis_[index] << NEWLINE_CHAR;
    }
  }
}//namespace utils
