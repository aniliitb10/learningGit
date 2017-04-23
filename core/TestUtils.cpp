#include "../include/Data.h"
#include <Windows.h>

int main()
{
  const std::string configFile("e:\\Workspace\\Eclipse\\EBSD-2.0\\additional\\sample.txt");
  const std::string dataFile("e:\\Workspace\\Eclipse\\EBSD-2.0\\additional\\data.txt");
  
  try
  {
    Data data(configFile, dataFile);

    // just to have a look at the stored values
    data.displayData("e:\\Workspace\\Eclipse\\EBSD-2.0\\additional\\data_out.txt");
  }
  catch (const Custom_Exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  system("pause");
}
