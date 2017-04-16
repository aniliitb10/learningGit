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
  _dataFileName(dataFileName_)
{
  initialize();
}

std::string Data::displayData() const
{
	std::ostringstream os;
	os << "phi1\tPHI\tphi2\tx\ty\tCI" << NEWLINE_CHAR;

	for (size_t index = 0; index < _dataSize; ++index)
	{
		os << _orientation[index][orientation::phi1] << "\t" << _orientation[index][orientation::PHI] << "\t"
			 << _orientation[index][orientation::phi2] << "\t" << _coordinates[index][coordinates::x] << "\t"
			 << _coordinates[index][coordinates::y]    << "\t" << _confidenceIndex[index] << NEWLINE_CHAR;
	}
	return os.str();
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

void Data::handleDisForEdgePoints()
{
	const auto& orientationItr = std::find_if(_orientation.begin(), _orientation.end(),
																						[&](const auto& elem_) {return elem_[coordinates::y] > ZERO_DOUBLE;});

	CUSTOM_ASSERT(orientationItr != _orientation.end(), "Couldn't find non-zero y coordinate in data");

	const size_t points_on_x_axis = std::distance(orientationItr, _orientation.begin()) + 1; //including the last element
	CUSTOM_ASSERT((_dataSize % points_on_x_axis == 0), "Number of points on x-axis doesn't divide the total number of points");

	auto disItrBegin = _disOrientation.begin();
	auto disItrEnd = _disOrientation.end();

	//taking care of bottom line (except first and last point)
	std::for_each(std::next(disItrBegin), std::next(disItrBegin, (points_on_x_axis - 1)),
																				[&](auto& elem){ elem[position::leftDown] = CUSTOM_NAN_DIS;
																												 elem[position::rightDown] = CUSTOM_NAN_DIS;
																												});

	//taking care of the top most line (except first and last point)
	std::for_each(std::prev(disItrEnd, (points_on_x_axis + 1)), std::prev(disItrEnd),
																				[&](auto& elem){ elem[position::leftUp] = CUSTOM_NAN_DIS;
																												 elem[position::rightUp] = CUSTOM_NAN_DIS;
																												});

	//taking care of the first column (except first and last point)
	for (auto itr = std::next(_disOrientation.begin(), points_on_x_axis); itr != _disOrientation.end(); ++itr)
	{
		_disOrientation[position::left] = CUSTOM_NAN_DIS;
		_disOrientation[position::leftDown] = CUSTOM_NAN_DIS;
		_disOrientation[position::leftUp] = CUSTOM_NAN_DIS;

		itr = std::next(itr, points_on_x_axis);
	}

	//taking care of the last column (except first and last point)
	for (auto itr = std::next(_disOrientation.begin(), (points_on_x_axis -1)); itr != _disOrientation.end(); ++itr)
	{
		_disOrientation[position::right] = CUSTOM_NAN_DIS;
		_disOrientation[position::rightDown] = CUSTOM_NAN_DIS;
		_disOrientation[position::rightUp] = CUSTOM_NAN_DIS;

		itr = std::next(itr, points_on_x_axis);
	}

	// now, taking care of the four points at the four corners
	// at the left bottom corner
	_disOrientation[0][left] = CUSTOM_NAN_DIS;
	_disOrientation[0][leftUp] = CUSTOM_NAN_DIS;
	_disOrientation[0][leftDown] = CUSTOM_NAN_DIS;
	_disOrientation[0][rightDown] = CUSTOM_NAN_DIS;

	// at the right bottom corner
	_disOrientation[points_on_x_axis-1][right] = CUSTOM_NAN_DIS;
	_disOrientation[points_on_x_axis-1][rightUp] = CUSTOM_NAN_DIS;
	_disOrientation[points_on_x_axis-1][rightDown] = CUSTOM_NAN_DIS;
	_disOrientation[points_on_x_axis-1][leftDown] = CUSTOM_NAN_DIS;

	// at the left top corner
	_disOrientation[_dataSize-points_on_x_axis][left] = CUSTOM_NAN_DIS;
	_disOrientation[_dataSize-points_on_x_axis][leftUp] = CUSTOM_NAN_DIS;
	_disOrientation[_dataSize-points_on_x_axis][leftDown] = CUSTOM_NAN_DIS;
	_disOrientation[_dataSize-points_on_x_axis][rightUp] = CUSTOM_NAN_DIS;

	// at the right top corner
	_disOrientation[_dataSize-1][right] = CUSTOM_NAN_DIS;
	_disOrientation[_dataSize-1][rightUp] = CUSTOM_NAN_DIS;
	_disOrientation[_dataSize-1][rightDown] = CUSTOM_NAN_DIS;
	_disOrientation[_dataSize-1][leftUp] = CUSTOM_NAN_DIS;
}
