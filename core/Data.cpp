#include "../include/Data.h"

#include <fstream>

namespace
{
/**
 * Reads the data file to determine the number of good lines (pure data lines)
 * and the number of bad lines (comments, beginning with #, lines that don't contain any data)
 * @param dataFileName_: the filename (with full path)
 * @return a struct Lines, having number of good line and bad lines
 * */
  const Lines countDataLines(const std::string& dataFileName_)
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
             &tmpIQ[ZERO], &_confidenceIndex[dataCounter]);
      dataCounter++;
    }
  }
}

void Data::initialize()
{
  const Lines lines = countDataLines(_dataFileName);
  _dataSize = lines.goodLines;

  _disOrientation.resize(_dataSize, std::vector<double>(position::lengthPosition));
  _coordinates.resize(_dataSize, std::vector<double>(coordinates::lengthCoordinates));
  _orientation.resize(_dataSize, std::vector<double>(orientation::lengthOrientation));
  _confidenceIndex.resize(_dataSize);

  initializeData(lines);
  std::cout << "Done here" << std::endl;
}
