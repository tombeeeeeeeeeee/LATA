#include "Utilities.h"

#include <iostream>
#include <sstream>
#include <fstream>

std::string FileToString(std::string path)
{
	std::stringstream output;
	std::ifstream file(path);

	if (file.is_open())
	{
		while (!file.eof())
		{
			std::string thisLine;
			std::getline(file, thisLine);
			output << thisLine << "\n";
		}
		return output.str();
	}
	else
	{
		std::cout << "Error loading file: " << path << std::endl;
		return "";
	}
}
