#pragma once

#include "MyUtils.h"
#include "Config.h"
#include <vector>

class Data
{
public:
  Data(const std::string& configFileName_, const std::string& dataFileName_);

private:
  // called by constructor
  // initialize must call labelGrains and calcDisorientation
  void initialize();

  // initializes the data member variables; initialize() internally calls this function
  void initializeData(const Lines& lines_);

  // to label the grains with their grain ids
  void labelGrains();

  // to calculate the misorientation between neighboring pixels and store them in _disOrientation
  void calcDisorientation();

  std::vector <std::vector <double>> _coordinates;
  std::vector <std::vector <double>> _orientation;
  std::vector <double> _confidenceIndex;
  std::vector <std::vector <double>> _disOrientation;
 
  Config _config;
  std::string _dataFileName;
  size_t _dataSize;

};
