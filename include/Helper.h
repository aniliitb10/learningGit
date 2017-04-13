#pragma once
#include <iostream>
#include <regex>
#include <map>
//list of all global static consts

static const int ZERO{ 0 };
static const unsigned int READLINE_LIMIT{ 253 };

static const char SPACE_CHAR(' ');
static const char NEWLINE_CHAR{ '\n' };
static const char SKIP_CHAR{ '#' };

static const std::string EMPTY_STR("");
static const std::string DUMMY_STR("DUMMY_STR");
//static const std::string DIGITS("0123456789");

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


//static const std::regex INT_TYPE(POS_NEG_REG_STR_BASIC + INT_TYPE_REG_STR_BASIC);
//static const std::regex UNSIGNED_INT_TYPE(POS_REG_STR_BASIC + INT_TYPE_REG_STR_BASIC);
//static const std::regex DOUBLE_TYPE(POS_NEG_REG_STR_BASIC + INT_TYPE_REG_STR_BASIC + DEC_REG_STR_BASIC + INT_TYPE_REG_STR_BASIC);
//static const std::regex UNSIGNED_DOUBLE_TYPE(POS_REG_STR_BASIC + INT_TYPE_REG_STR_BASIC + DEC_REG_STR_BASIC + INT_TYPE_REG_STR_BASIC);

//static const std::regex INT_TYPE("[+-]?" + INT_TYPE_REG_STR_BASIC);
//static const std::regex UNSIGNED_INT_TYPE("[+]?[0-9]+");
//static const std::regex DOUBLE_TYPE("[+-]?[0-9]+[.]?[0-9]+");
//static const std::regex UNSIGNED_DOUBLE_TYPE("[+]?[0-9]+[.]?[0-9]+");

//list of all typedef/using
using strVec = std::vector<std::string>;
using strToStrMap = std::map<std::string, std::string>;
using sizetPair = std::pair<size_t, size_t>;

struct Lines
{
  Lines() : goodLines(0), badLines(0)
  {
  }

  size_t goodLines;
  size_t badLines;
};