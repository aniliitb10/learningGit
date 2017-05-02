#include "../include/Data.h"
#include <Windows.h>

int main()
{
  const std::string configFile("d:\\Workspace\\Eclipse\\EBSD 2.0 Files\\additional\\sample.txt");
  //const std::string dataFile("d:\\Workspace\\Eclipse\\EBSD-2.0\\additional\\data.txt");
  const std::string dataFile("d:\\Workspace\\Eclipse\\EBSD 2.0 Files\\additional\\grain file new.txt");
  
  try
  {
    Data data(configFile, dataFile);

    // just to have a look at the stored values
    data.displayData("d:\\Workspace\\Eclipse\\EBSD 2.0 Files\\additional\\data_out.txt");
  }
  catch (const Custom_Exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  system("pause");
}
