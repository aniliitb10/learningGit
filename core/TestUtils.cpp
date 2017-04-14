#include "../include/Data.h"
#include "../include/MyUtils.h"

int main()
{
  const std::string configFile("e:\\Workspace\\Eclipse\\EBSD-2.0\\additional\\sample.txt");
  const std::string dataFile("e:\\Workspace\\Eclipse\\EBSD-2.0\\additional\\data.txt");
	Data data(configFile, dataFile);

	// just to have a look at the stored values
	std::cout << data.displayData() << std::endl;
}
