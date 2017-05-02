#include "../include/Data.h"

#include <fstream>
#include <algorithm>
#include <numeric>

namespace
{
/**
 * Reads the data file to determine the number of good lines (pure data lines)
 * and the number of bad lines (comments, beginning with #, lines that don't contain any data)
 * @param dataFileName_: the filename (with full path)
 * @return a struct Lines, having number of good line and bad lines
 * */
  Lines countDataLines(const std::string& dataFileName_)
  {
    Lines lines;
    unsigned int goodlLines{ 0 }, badLines{ 0 };

    std::ifstream dataFileHandle(dataFileName_, std::ios::in);
    if (dataFileHandle.good())
    {
      std::string rawStr;
      std::locale loc;

      while (std::getline(dataFileHandle >> std::ws, rawStr))
      {
        if (std::isdigit(rawStr[0], loc))
        {
          goodlLines = 1;
          break;
        }
        badLines++;
      }
      while (std::getline(dataFileHandle, rawStr))
        goodlLines++;

      lines.goodLines = goodlLines;
      lines.badLines = badLines;

      return lines;
    } //data file handle is not good
    else
    {
      std::cerr << "Couldn't read the file in function: countGoodDataLines" << std::endl;
      return lines;
    }
  }
}

Data::Data(const std::string& configFileName_, const std::string& dataFileName_):
  _config(configFileName_),
  _dataFileName(dataFileName_),
  _dataSize(CUSTOM_NAN_SIZE_T),
  _numOfPointsInTheBottomRow(CUSTOM_NAN_SIZE_T)
{
  initialize();
}

void Data::displayData(const std::string& filePath_) const
{
  std::fstream outputFileHandle(filePath_, std::ios::out);

  for (size_t index = 0; index < _disOrientation.size(); ++index)
  {
    for (size_t subIndex = 0; subIndex < position::lengthPosition; ++subIndex)
    {
      outputFileHandle << _disOrientation[index][subIndex] << "\t\t";
    }
    outputFileHandle << NEWLINE_CHAR;
  }
}

void Data::initializeData(const Lines& lines_)
{
  std::ifstream dataFileHandle(_dataFileName, std::ios::in);
  if (dataFileHandle.good())
  {
    std::string rawStr;
    size_t lineCounter{ 0 }, dataCounter{ 0 };
    std::string tmpIQ(DUMMY_STR);

    while (lineCounter < lines_.badLines)
    {
      std::getline(dataFileHandle, rawStr);
      lineCounter++;
    }

    while (std::getline(dataFileHandle >> std::ws, rawStr))
    {
      sscanf(rawStr.c_str(), "%lf %lf %lf %lf %lf %s %lf",
             &_orientation[dataCounter][orientation::phi1], &_orientation[dataCounter][orientation::PHI], &_orientation[dataCounter][orientation::phi2],
             &_coordinates[dataCounter][coordinates::x], &_coordinates[dataCounter][coordinates::y],
             &tmpIQ[ZERO_INT], &_confidenceIndex[dataCounter]);
      dataCounter++;
    }

    std::iota(_id.begin(), _id.end(), 0);
    std::fill(_size.begin(), _size.end(), 1);

    calcDisorientationAndConnectGrains();
    generateResults();
  }
  else
  {
    throw Custom_Exception("Couldn't read the data file");
  }
}

void Data::initialize()
{
  const auto lines = countDataLines(_dataFileName);
  _dataSize = lines.goodLines;

  _disOrientation.resize(_dataSize, std::vector<double>(position::lengthPosition, CUSTOM_NAN_DOUBLE_MIN));
  _coordinates.resize(_dataSize, std::vector<double>(coordinates::lengthCoordinates, CUSTOM_NAN_DOUBLE_MIN));
  _orientation.resize(_dataSize, std::vector<double>(orientation::lengthOrientation, CUSTOM_NAN_DOUBLE_MIN));
  _confidenceIndex.resize(_dataSize, CUSTOM_NAN_DOUBLE_MIN);
  _id.resize(_dataSize);
  _size.resize(_dataSize);

  initializeData(lines);
}


double Data::misorientation(size_t index1, size_t index2)
{
  return utils::misorientation(_orientation[index1],
                               _orientation[index2],
                               _confidenceIndex[index1],
                               _confidenceIndex[index2]);
}

void Data::calcDisorientationAndConnectGrains()
{
  std::cout << "Inside Data::calcDisorientationAndConnectGrains" << std::endl;
  
  // start with assigning CUSTOM_NULL values to the points at the edges
  handleDisForEdgePoints();
  const double misorientationThreshold = _config.getMisorientation();

  const Neighbors neighbours(_numOfPointsInTheBottomRow);

  for (size_t index = 0; index < _dataSize; ++index)
  {
    if (_disOrientation[index][position::left] != CUSTOM_NAN_DOUBLE_MAX)
    {
      _disOrientation[index][position::left] = _disOrientation[index - neighbours.left][position::right];
    }

    if (_disOrientation[index][position::right] != CUSTOM_NAN_DOUBLE_MAX)
    {
      _disOrientation[index][position::right] = misorientation(index, index + neighbours.right);
      
      if (_disOrientation[index][position::right] < misorientationThreshold)
      {
        utils::unionPoints(_id, _size, index, (index + neighbours.right));
      }
    }

    if (_disOrientation[index][position::leftUp] != CUSTOM_NAN_DOUBLE_MAX)
    {
      _disOrientation[index][position::leftUp] = misorientation(index, index + neighbours.leftUp);
      if (_disOrientation[index][position::leftUp] < misorientationThreshold)
      {
        utils::unionPoints(_id, _size, index, (index + neighbours.leftUp));
      }
    }

    if (_disOrientation[index][position::rightUp] != CUSTOM_NAN_DOUBLE_MAX)
    {
      _disOrientation[index][position::rightUp] = misorientation(index, index + neighbours.rightUp);
      if (_disOrientation[index][position::rightUp] < misorientationThreshold)
      {
        utils::unionPoints(_id, _size, index, (index + neighbours.rightUp));
      }
    }

    if (_disOrientation[index][position::leftDown] != CUSTOM_NAN_DOUBLE_MAX)
    {
      _disOrientation[index][position::leftDown] = _disOrientation[index - neighbours.leftDown][position::rightUp];
    }

    if (_disOrientation[index][position::rightDown] != CUSTOM_NAN_DOUBLE_MAX)
    {
      _disOrientation[index][position::rightDown] = _disOrientation[index - neighbours.rightDown][position::leftUp];
    }
  }
}

void Data::handleDisForEdgePoints()
{
  const auto& firstPointOf2ndRowItr = std::find_if(_coordinates.begin(), _coordinates.end(),
                                            [](const auto& elem_) {return elem_[coordinates::y] > ZERO_DOUBLE;});

  CUSTOM_ASSERT(firstPointOf2ndRowItr != _coordinates.end(), "Couldn't find non-zero y coordinate in data");

  _numOfPointsInTheBottomRow = std::distance(_coordinates.begin(), std::prev(firstPointOf2ndRowItr)) + 1; //including the last element
  
  std::cout << "Number of points in the bottomRow: " << _numOfPointsInTheBottomRow << NEWLINE_CHAR;

  std::cout << "Total number of points: " << _dataSize << NEWLINE_CHAR;

  CUSTOM_ASSERT(((_dataSize % (2 * _numOfPointsInTheBottomRow -1 ) == 0) ||
               ((_dataSize % (2 * _numOfPointsInTheBottomRow - 1) == _numOfPointsInTheBottomRow))),
                "total number of points do not make sense!");

  PointsStructure pointsStructure = (((_dataSize % (2 * _numOfPointsInTheBottomRow - 1) == 0) ? evenNumbered : oddNumbered));

  //taking care of bottom line (except first and last point)
  std::for_each(std::next(_disOrientation.begin()),
                std::next(_disOrientation.begin(), (_numOfPointsInTheBottomRow - 1)),
                [&](auto& elem){ elem[position::leftDown] = CUSTOM_NAN_DOUBLE_MAX;
                                 elem[position::rightDown] = CUSTOM_NAN_DOUBLE_MAX;
                               });

  // evenNumbered: the number of points in the top row is one less than that in bottom 
  // oddNumbered: the number of points in the top row is equal to that in bottom row
  const auto& firstPointOfLastRowItr = (pointsStructure == evenNumbered ? std::prev(_disOrientation.end(), (_numOfPointsInTheBottomRow - 1))
                                                                        : std::prev(_disOrientation.end(), (_numOfPointsInTheBottomRow)));

  //taking care of the top most line (except first and last point)
  std::for_each(std::next(firstPointOfLastRowItr), std::prev(_disOrientation.end()), //excludes the 2nd argument
                          [&](auto& elem){ elem[position::leftUp] = CUSTOM_NAN_DOUBLE_MAX;
                                            elem[position::rightUp] = CUSTOM_NAN_DOUBLE_MAX;
                                          });

  //taking care of the first column (except first and last point)
  for (size_t index = (2 * _numOfPointsInTheBottomRow - 1); index < _dataSize;)
  {
    _disOrientation[index][position::left] = CUSTOM_NAN_DOUBLE_MAX;
    _disOrientation[index][position::leftDown] = CUSTOM_NAN_DOUBLE_MAX;
    _disOrientation[index][position::leftUp] = CUSTOM_NAN_DOUBLE_MAX;

    index += (2 * _numOfPointsInTheBottomRow - 1);
  }

  //taking care of the last column (except first and last point)
  for (size_t index = (3 * _numOfPointsInTheBottomRow - 2); index < _dataSize;)
  {
    _disOrientation[index][position::right] = CUSTOM_NAN_DOUBLE_MAX;
    _disOrientation[index][position::rightDown] = CUSTOM_NAN_DOUBLE_MAX;
    _disOrientation[index][position::rightUp] = CUSTOM_NAN_DOUBLE_MAX;

    index += (2 * _numOfPointsInTheBottomRow - 1);
  }

  // now, taking care of the four points at the four corners
  // at the left bottom corner
  _disOrientation[0][left] = CUSTOM_NAN_DOUBLE_MAX;
  _disOrientation[0][leftUp] = CUSTOM_NAN_DOUBLE_MAX;
  _disOrientation[0][leftDown] = CUSTOM_NAN_DOUBLE_MAX;
  _disOrientation[0][rightDown] = CUSTOM_NAN_DOUBLE_MAX;

  // at the right bottom corner
  _disOrientation[_numOfPointsInTheBottomRow-1][right] = CUSTOM_NAN_DOUBLE_MAX;
  _disOrientation[_numOfPointsInTheBottomRow-1][rightUp] = CUSTOM_NAN_DOUBLE_MAX;
  _disOrientation[_numOfPointsInTheBottomRow-1][rightDown] = CUSTOM_NAN_DOUBLE_MAX;
  _disOrientation[_numOfPointsInTheBottomRow-1][leftDown] = CUSTOM_NAN_DOUBLE_MAX;

  // at the left top corner (careful, the number of points in this row 'could be' one less than that in bottom row)
  // (and it has its left down neighbour)
  _disOrientation[_dataSize - (_numOfPointsInTheBottomRow -1) - pointsStructure][left] = CUSTOM_NAN_DOUBLE_MAX;
  _disOrientation[_dataSize - (_numOfPointsInTheBottomRow -1) - pointsStructure][leftUp] = CUSTOM_NAN_DOUBLE_MAX;
  _disOrientation[_dataSize - (_numOfPointsInTheBottomRow -1) - pointsStructure][rightUp] = CUSTOM_NAN_DOUBLE_MAX;
  if (pointsStructure == PointsStructure::oddNumbered)
  {
    _disOrientation[_dataSize - _numOfPointsInTheBottomRow][leftDown] = CUSTOM_NAN_DOUBLE_MAX;
  }

  // at the right top corner (it has its rightDown neighbour)
  _disOrientation[_dataSize-1][right] = CUSTOM_NAN_DOUBLE_MAX;
  _disOrientation[_dataSize-1][rightUp] = CUSTOM_NAN_DOUBLE_MAX;
  _disOrientation[_dataSize-1][leftUp] = CUSTOM_NAN_DOUBLE_MAX;
  if (pointsStructure == PointsStructure::oddNumbered)
  {
    _disOrientation[_dataSize - 1][rightDown] = CUSTOM_NAN_DOUBLE_MAX;
  }

  // 2nd column from left end (no left neighbour)
  for (size_t index = _numOfPointsInTheBottomRow; index < _dataSize;)
  {
    _disOrientation[index][left] = CUSTOM_NAN_DOUBLE_MAX;
    index += (2 * _numOfPointsInTheBottomRow - 1);
  }

  // 2nd column from right end (no right neighbour) 
  for (size_t index = (2 * _numOfPointsInTheBottomRow - 2); index < _dataSize;)
  {
    _disOrientation[index][right] = CUSTOM_NAN_DOUBLE_MAX;
    index += (2 * _numOfPointsInTheBottomRow - 1);
  }
}

void Data::generateResults()
{
  //rightnow, just hard coding. Will figure out a way to get it by user later
  calcGSD();
}

void Data::calcGSD()
{
  size_t maxId = *(std::max_element(_id.begin(), _id.end()));
  
  size_tVec grainSize(maxId + 1);
  doubleVec diameter;

  const double minConfidenceIndex = _config.getMinConfidenceIndex();
  const size_t minGrainSize = std::max(_config.getMinGrainSize(), static_cast<size_t>(1));

  const double stepSize = _coordinates[1][coordinates::x] - _coordinates[0][coordinates::x];

  for (size_t index = 0; index < _size.size(); ++index)
  {
    if (_confidenceIndex[index] >= minConfidenceIndex)
    {
      grainSize[_id[index]] += 1;
    }
  }

  for (size_t index = 0; index < grainSize.size(); ++index)
  {
    if (grainSize[index] >= minGrainSize)
    {
      diameter.push_back(1.05*stepSize*sqrt(grainSize[index]));
    }
  }

  doubleVec xAxis(_config.getNumOfBins());
  size_tVec yAxis(_config.getNumOfBins());

  utils::binnyfy(diameter, yAxis, xAxis);
  utils::writeDataToFile("d:\\Workspace\\Eclipse\\EBSD 2.0 Files\\additional\\gsd.txt", yAxis, xAxis);
}
