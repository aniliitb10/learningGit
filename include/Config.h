#pragma once

#include "MyUtils.h"
#include <sstream>
#include <functional>

class Config
{
public:
  /**
   * to pass the verification functions in  Config::configCheck
   * */
  using check = std::function<bool(const std::string&)>;

  /**
   * Initializes the config with default values
   * */
  Config(const std::string& fileName_);
  
  /**
   * prints the errors encountered while reading the config
   * @param fileName_: the file where it prints the errors
   * */
  void printErrors(const std::string& fileName_);
 
  const std::string& getFileName() const { return _fileName; };
  const double& getMinConfidenceIndex() const { return _minConfidenceIndex; };
  const double& getMisorientation() const { return _misorientation; };
  const size_t& getMinGrainSize() const { return _minGrainSize; };
  const size_t& getNumOfBins() const { return _numOfBins; };

private:
  /**
   * called by constructor
   * reads the configFile and assigns data (tag values) to the member variables (tags)
   * */
  bool initialize();

  /**
   * validates the (type of) data read from config file
   * and reports errors, if any (stores in _configError)
   * @param tag_: the tag, whose values is being verified
   * @param defaultValue_: the default value assigned in the constructor
   * @param typeCheck_: the function which verifies that the tagValue_ read from config is of the correct type
   * @param tagValue_: the value of the tag_ which was read from config
   * @return True: if the tagValue_ is of the expected type, False otherwise
   * */
  template <typename T>
  bool configCheck(const std::string& tag_, const T& defaultValue_, const check& typeCheck_, std::string& tagValue_)
  {
    tagValue_ = _tagToValueMap[tag_];
    bool checkResult = typeCheck_(tagValue_);
    if (!checkResult)
    {
      std::ostringstream os;
      os << "Invalid value " << tagValue_ << " of " << tag_ << SPACE_CHAR
        << "Proceeding with its default value: " << defaultValue_ << NEWLINE_CHAR;

      _configError[tag_] = os.str();
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
