#include "../include/Data.h"

#include <fstream>
#include <algorithm>

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
  //std::ostringstream os;
  //os << "phi1\tPHI\tphi2\tx\ty\tCI" << NEWLINE_CHAR;

  //for (size_t index = 0; index < _dataSize; ++index)
  //{
  //  os << _orientation[index][orientation::phi1] << "\t" << _orientation[index][orientation::PHI] << "\t"
  //     << _orientation[index][orientation::phi2] << "\t" << _coordinates[index][coordinates::x] << "\t"
  //     << _coordinates[index][coordinates::y]    << "\t" << _confidenceIndex[index] << NEWLINE_CHAR;
  //}
  //return os.str();

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
  }

  calcDisorientation();
}

void Data::initialize()
{
  const auto lines = countDataLines(_dataFileName);
  _dataSize = lines.goodLines;

  _disOrientation.resize(_dataSize, std::vector<double>(position::lengthPosition));
  _coordinates.resize(_dataSize, std::vector<double>(coordinates::lengthCoordinates));
  _orientation.resize(_dataSize, std::vector<double>(orientation::lengthOrientation));
  _confidenceIndex.resize(_dataSize);

  initializeData(lines);
}


double Data::misorientation(size_t index1, size_t index2)
{
  return utils::misorientation(_orientation[index1],
                               _orientation[index2],
                               _confidenceIndex[index1],
                               _confidenceIndex[index2]);
}

void Data::calcDisorientation()
{
  std::cout << "Inside Data::calcDisorientation()" << std::endl;
  // start with assigning CUSTOM_NULL values to the points at the edges
  handleDisForEdgePoints();

  const Neighbors neighbours(_numOfPointsInTheBottomRow);

  for (size_t index = 0; index < _dataSize; ++index)
  {
    if ((_disOrientation[index][position::left] != CUSTOM_NAN_DOUBLE) &&
      (_disOrientation[index - neighbours.left][position::right] != CUSTOM_NAN_DOUBLE))
    {
      _disOrientation[index][position::left] = misorientation(index, index - neighbours.left);
    }

    if ((_disOrientation[index][position::right] != CUSTOM_NAN_DOUBLE) &&
      (_disOrientation[index + neighbours.right][position::left] != CUSTOM_NAN_DOUBLE))
    {
      _disOrientation[index][position::right] = misorientation(index, index + neighbours.right);
    }

    if ((_disOrientation[index][position::leftUp] != CUSTOM_NAN_DOUBLE) &&
      (_disOrientation[index + neighbours.leftUp][position::rightDown] != CUSTOM_NAN_DOUBLE))
    {
      _disOrientation[index][position::leftUp] = misorientation(index, index + neighbours.leftUp);
    }

    if ((_disOrientation[index][position::rightUp] != CUSTOM_NAN_DOUBLE) &&
      (_disOrientation[index + neighbours.rightUp][position::leftDown] != CUSTOM_NAN_DOUBLE))
    {
      _disOrientation[index][position::rightUp] = misorientation(index, index + neighbours.rightUp);
    }

    if ((_disOrientation[index][position::leftDown] != CUSTOM_NAN_DOUBLE) &&
      (_disOrientation[index - neighbours.leftDown][position::rightUp] != CUSTOM_NAN_DOUBLE))
    {
      _disOrientation[index][position::leftDown] = misorientation(index, index - neighbours.leftDown);
    }

    if ((_disOrientation[index][position::rightDown] != CUSTOM_NAN_DOUBLE) &&
      (_disOrientation[index - neighbours.rightDown][position::leftUp] != CUSTOM_NAN_DOUBLE))
    {
      _disOrientation[index][position::rightDown] = misorientation(index, index - neighbours.rightDown);
    }
  }

}

void Data::handleDisForEdgePoints()
{
  const auto& firstPointOf2ndRowItr = std::find_if(_coordinates.begin(), _coordinates.end(),
                                            [](const auto& elem_) {return elem_[coordinates::y] > ZERO_DOUBLE;});
  

  CUSTOM_ASSERT(firstPointOf2ndRowItr != _coordinates.end(), "Couldn't find non-zero y coordinate in data");

  _numOfPointsInTheBottomRow = std::distance(_coordinates.begin(), std::prev(firstPointOf2ndRowItr)) + 1; //including the last element
  
  std::cout << "Number of points in the bottomRow: " << _numOfPointsInTheBottomRow << std::endl;

  CUSTOM_ASSERT((_dataSize % (2 * _numOfPointsInTheBottomRow -1 ) == 0),
                "total number of points is not a multiple of points in the first two rows");

  //taking care of bottom line (except first and last point)
  std::for_each(std::next(_disOrientation.begin()),
                std::next(_disOrientation.begin(), (_numOfPointsInTheBottomRow - 1)),
                [&](auto& elem){ elem[position::leftDown] = CUSTOM_NAN_DOUBLE;
                                 elem[position::rightDown] = CUSTOM_NAN_DOUBLE;
                               });

  // as the number of points in the top row is one less than that in bottom row
  const auto& firstPointOfLastRowItr = std::prev(_disOrientation.end(), (_numOfPointsInTheBottomRow - 1));

  //taking care of the top most line (except first and last point)
  std::for_each(std::next(firstPointOfLastRowItr), std::prev(_disOrientation.end()), //excludes the 2nd argument
                          [&](auto& elem){ elem[position::leftUp] = CUSTOM_NAN_DOUBLE;
                                            elem[position::rightUp] = CUSTOM_NAN_DOUBLE;
                                          });

  //taking care of the first column (except first and last point)
  for (size_t index = (2 * _numOfPointsInTheBottomRow - 1); index < _dataSize;)
  {
    _disOrientation[index][position::left] = CUSTOM_NAN_DOUBLE;
    _disOrientation[index][position::leftDown] = CUSTOM_NAN_DOUBLE;
    _disOrientation[index][position::leftUp] = CUSTOM_NAN_DOUBLE;

    index += (2 * _numOfPointsInTheBottomRow - 1);
  }

  //taking care of the last column (except first and last point)
  for (size_t index = (_numOfPointsInTheBottomRow - 1); index < _dataSize;)
  {
    _disOrientation[index][position::right] = CUSTOM_NAN_DOUBLE;
    _disOrientation[index][position::rightDown] = CUSTOM_NAN_DOUBLE;
    _disOrientation[index][position::rightUp] = CUSTOM_NAN_DOUBLE;

    index += (2 * _numOfPointsInTheBottomRow - 1);
  }

  // now, taking care of the four points at the four corners
  // at the left bottom corner
  _disOrientation[0][left] = CUSTOM_NAN_DOUBLE;
  _disOrientation[0][leftUp] = CUSTOM_NAN_DOUBLE;
  _disOrientation[0][leftDown] = CUSTOM_NAN_DOUBLE;
  _disOrientation[0][rightDown] = CUSTOM_NAN_DOUBLE;

  // at the right bottom corner
  _disOrientation[_numOfPointsInTheBottomRow-1][right] = CUSTOM_NAN_DOUBLE;
  _disOrientation[_numOfPointsInTheBottomRow-1][rightUp] = CUSTOM_NAN_DOUBLE;
  _disOrientation[_numOfPointsInTheBottomRow-1][rightDown] = CUSTOM_NAN_DOUBLE;
  _disOrientation[_numOfPointsInTheBottomRow-1][leftDown] = CUSTOM_NAN_DOUBLE;

  // at the left top corner (careful, the number of points in this row is one less than that in bottom row)
  // (and it has its left down neighbour)
  _disOrientation[_dataSize- (_numOfPointsInTheBottomRow -1)][left] = CUSTOM_NAN_DOUBLE;
  _disOrientation[_dataSize- (_numOfPointsInTheBottomRow -1)][leftUp] = CUSTOM_NAN_DOUBLE;
  _disOrientation[_dataSize- (_numOfPointsInTheBottomRow -1)][rightUp] = CUSTOM_NAN_DOUBLE;

  // at the right top corner (it has its rightDown neighbour)
  _disOrientation[_dataSize-1][right] = CUSTOM_NAN_DOUBLE;
  _disOrientation[_dataSize-1][rightUp] = CUSTOM_NAN_DOUBLE;
  _disOrientation[_dataSize-1][leftUp] = CUSTOM_NAN_DOUBLE;
}
