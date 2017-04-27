#pragma once

#include "Config.h"
#include <vector>

class Data
{
public:
  Data(const std::string& configFileName_, const std::string& dataFileName_);

  /**
   * to display the stored data
   * */
  void displayData(const std::string& filePath_) const;

private:
  /**
   * called by constructor
   * calls Data::initializeData to actually do the initialization
   * */
  void initialize();

  /**
   * called by Data::initialize
   * this function fills _coordinates, _orientation and _confidenceIndex from data file
   * @param lines_: a Lines struct having the number of bad lines and good lines
   * */
  void initializeData(const Lines& lines_);

  /**
   * to label the grains with their grain ids
   * */
  void labelGrains();

  /**
   * to calculate the misorientation between neighboring pixels and
   * store them in _disOrientation
   * */
  void calcDisorientation();


  /**
  * actual calculation of Disorientation
  **/

  double misorientation(size_t index1, size_t index_2);

  /**
   * Called by calcDisorientation()
   * fills the disorientation values for points at boundaries by CUSTOM_NAN
   * */
  void handleDisForEdgePoints();

  std::vector <std::vector <double>> _coordinates;
  std::vector <std::vector <double>> _orientation;
  std::vector <double> _confidenceIndex;
  std::vector <std::vector <double>> _disOrientation;
  std::vector <size_t> _size;
  std::vector <size_t> _id;
 
  Config _config;
  std::string _dataFileName;
  size_t _dataSize;
  size_t _numOfPointsInTheBottomRow;
};
