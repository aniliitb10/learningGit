#include "../include/Data.h"
#include "../include/MyUtils.h"
#include <windows.h>
//#include "Config.h" 

static const std::string MIN_CONFIDENCE_INDEX("Minimum confidence index");
static const std::string MISORIENTATION("Misorientation");
static const std::string MIN_GRAIN_SIZE("minimum grain size");
static const std::string NUM_OF_BINS("Number of bins");

int main()
{
  //strToStrMap tagToValueMap = { {MIN_CONFIDENCE_INDEX, EMPTY_STR},
  //                              {MISORIENTATION, EMPTY_STR },
  //                              {MIN_GRAIN_SIZE, EMPTY_STR },
  //                              {NUM_OF_BINS, EMPTY_STR } };
  //utils::getTagsFromConfig("C:\\Users\\kumaa\\Documents\\Visual Studio 2015\\Projects\\CppConsoleApps\\CppConsoleApps\\sample.txt", tagToValueMap);
  //utils::print(listOfValues, NEWLINE_CHAR);
  //testNumericTypes();

  const std::string configFile("c:\\users\\kumaa\\documents\\visual studio 2015\\projects\\CppConsoleApps\\CppConsoleApps\\sample.txt");
  const std::string dataFile("c:\\users\\kumaa\\documents\\visual studio 2015\\projects\\CppConsoleApps\\CppConsoleApps\\data.txt");
  Data data(configFile, dataFile);
//  char filePath[256];
//  GetCurrentDirectoryA(256, filePath);
//  std::cout << "the current directory is: " << filePath << std::endl;



  //data.initialize();

  //Config config(file);
  //config.intialize();






  //std::cout << config.getMinGrainSize() << NEWLINE_CHAR;
  //config.printErrors(file);

  //std::string hundrd("100");

  //bool result = utils::isUnsignedIntegerType(hundrd);

  system("pause");
}
