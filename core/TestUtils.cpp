#include "../include/Data.h"
#include "../include/MyUtils.h"
#include <windows.h>

int main()
{
  const std::string configFile("c:\\users\\kumaa\\documents\\visual studio 2015\\projects\\CppConsoleApps\\CppConsoleApps\\sample.txt");
  const std::string dataFile("c:\\users\\kumaa\\documents\\visual studio 2015\\projects\\CppConsoleApps\\CppConsoleApps\\data.txt");
	Data data(configFile, dataFile);
}
