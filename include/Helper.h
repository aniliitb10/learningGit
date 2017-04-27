/**
 * This file contains:
 * - all type aliases (using)
 * - all constants
 * */

#pragma once

#include <iostream>
#include <regex>
#include <map>
#include <limits>

static constexpr int ZERO_INT { 0 };
static constexpr double ZERO_DOUBLE { 0.00 };
static constexpr unsigned int READLINE_LIMIT { 253 };
static constexpr double CUSTOM_NAN_DOUBLE_MAX {std::numeric_limits<double>::max()};
static constexpr double CUSTOM_NAN_DOUBLE_MIN { std::numeric_limits<double>::min() };
static constexpr size_t CUSTOM_NAN_SIZE_T {std::numeric_limits<size_t>::max()};

static constexpr double PI { 3.14159265358979323846 };

static constexpr char SPACE_CHAR(' ');
static constexpr char NEWLINE_CHAR{ '\n' };
static constexpr char SKIP_CHAR{ '#' };

static const std::string EMPTY_STR("");
static const std::string DUMMY_STR("DUMMY_STR");

//these are basic for building high level objects
static const std::string POS_REG_STR_BASIC("[+]?");
static const std::string POS_NEG_REG_STR_BASIC("[+-]?");
static const std::string INT_TYPE_REG_STR_BASIC("[0-9]+");
static const std::string DEC_REG_STR_BASIC("[.]?");
static const std::string SPACE_STR_BASIC("[\s]*");

static const std::string INT_TYPE_STR(POS_NEG_REG_STR_BASIC + INT_TYPE_REG_STR_BASIC);
static const std::string UNSIGNED_INT_TYPE_STR(POS_REG_STR_BASIC + INT_TYPE_REG_STR_BASIC);
static const std::string DOUBLE_TYPE_STR(POS_NEG_REG_STR_BASIC + INT_TYPE_REG_STR_BASIC + DEC_REG_STR_BASIC + INT_TYPE_REG_STR_BASIC);
static const std::string UNSIGNED_DOUBLE_TYPE_STR(POS_REG_STR_BASIC + INT_TYPE_REG_STR_BASIC + DEC_REG_STR_BASIC + INT_TYPE_REG_STR_BASIC);

static const std::regex INT_TYPE(INT_TYPE_STR);
static const std::regex UNSIGNED_INT_TYPE(UNSIGNED_INT_TYPE_STR);
static const std::regex DOUBLE_TYPE(DOUBLE_TYPE_STR);
static const std::regex UNSIGNED_DOUBLE_TYPE(UNSIGNED_DOUBLE_TYPE_STR);

using strVec = std::vector<std::string>;
using strToStrMap = std::map<std::string, std::string>;
using sizetPair = std::pair<size_t, size_t>;
using doubleVec = std::vector<double>;
using size_tVec = std::vector<size_t>;

struct Lines
{
  Lines() : goodLines(0), badLines(0)
  {
  }

  size_t goodLines;
  size_t badLines;
};
