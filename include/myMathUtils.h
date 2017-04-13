#pragma once
#include "Helper.h"

namespace utils
{
  static bool isIntegerType(const std::string& str_)
  {
    return std::regex_match(str_, INT_TYPE);
  }

  static bool isUnsignedIntegerType(const std::string& str_)
  {
    return std::regex_match(str_, UNSIGNED_INT_TYPE);
  }

  static bool isDoubleType(const std::string& str_)
  {
    return std::regex_match(str_, DOUBLE_TYPE);
  }

  static bool isUnsignedDoubleType(const std::string& str_)
  {
    return std::regex_match(str_, UNSIGNED_DOUBLE_TYPE);
  }
}