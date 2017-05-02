#include "../include/Config.h"

static const std::string MIN_CONFIDENCE_INDEX("confidence");
static const std::string MISORIENTATION("misorientation");
static const std::string MIN_GRAIN_SIZE("size");
static const std::string NUM_OF_BINS("bins");

Config::Config(const std::string& fileName_) :
  _fileName(fileName_),
  _minConfidenceIndex(0.1),
  _misorientation(5.0),
  _minGrainSize(2),
  _numOfBins(20)
{
  initialize();
}

bool Config::initialize()
{
  _tagToValueMap = {
                     { MIN_CONFIDENCE_INDEX, EMPTY_STR },
                     { MISORIENTATION, EMPTY_STR },
                     { MIN_GRAIN_SIZE, EMPTY_STR },
                     { NUM_OF_BINS, EMPTY_STR }
                   };

  utils::getTagsFromConfig(_fileName, _tagToValueMap);
  if (_tagToValueMap.empty())
  {
    std::cerr << "ERROR: Couldn't get the values from tags" << NEWLINE_CHAR;
    return false;
  }

  std::string tagValue;
  //confidence index
  if (configCheck(MIN_CONFIDENCE_INDEX, _minConfidenceIndex, utils::isDoubleType, tagValue))
  {
    _minConfidenceIndex = std::stod(tagValue);
  }

  //misorientation for grain boundary
  if (configCheck(MISORIENTATION, _misorientation, utils::isUnsignedDoubleType, tagValue))
  {
    _misorientation = std::stod(tagValue);
  }

  //minimum grain size
  if (configCheck(MIN_GRAIN_SIZE, _minGrainSize, utils::isUnsignedIntegerType, tagValue))
  {
    _minGrainSize = std::stoi(tagValue);
  }

  //number of bins
  if (configCheck(NUM_OF_BINS, _numOfBins, utils::isUnsignedIntegerType, tagValue))
  {
    _numOfBins = std::stoi(tagValue);
  }

  return true;
}

void Config::printErrors(const std::string& fileName_)
{
  if (!_configError.empty())
  {
    std::ostringstream os;
    os << "Following are the errors encountered while reading the config file \"" << _fileName << "\"" << NEWLINE_CHAR;
    
    for (auto configErrorItr = _configError.cbegin(); configErrorItr != _configError.cend(); configErrorItr++)
    {
      os << configErrorItr->first << " : " << configErrorItr->second << NEWLINE_CHAR;
    }

    std::ofstream file(fileName_, std::ios::out | std::ios::app);
    file << os.str() << std::endl;
  }
}
