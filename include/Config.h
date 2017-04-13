#pragma once
#include "MyUtils.h"
#include <sstream>
#include <functional>

class Config
{
public:
  using check = std::function<bool(const std::string&)>;

  Config(const std::string& fileName_);
  
  void printErrors(const std::string& fileName_);
 
  const std::string& getFileName() const { return _fileName; };
  const double& getMinConfidenceIndex() const { return _minConfidenceIndex; };
  const double& getMisorientation() const { return _misorientation; };
  const size_t& getMinGrainSize() const { return _minGrainSize; };
  const size_t& getNumOfBins() const { return _numOfBins; };

private:
  // called by constructor, reads the configFile and assigns data to the member variables
  bool initialize();

  template <typename T>
  bool configCheck(const std::string& variable_, const T& defaultValue_, const check& typeCheck_, std::string& tagValue_)
  {
    tagValue_ = _tagToValueMap[variable_];
    bool checkResult = typeCheck_(tagValue_);
    if (!checkResult)
    {
      std::ostringstream os;
      os << "Invalid value " << tagValue_ << " of " << variable_ << SPACE_CHAR
        << "Proceeding with its default value: " << defaultValue_ << NEWLINE_CHAR;

      _configError[variable_] = os.str();
      std::cerr << os.str();
    }
    return checkResult;
  }

  std::string _fileName;
  double _minConfidenceIndex;
  double _misorientation;
  size_t _minGrainSize;
  size_t _numOfBins;
  strToStrMap _configError;
  strToStrMap _tagToValueMap;
};