#include "Utilities.h"

#include <iostream>
#include <sstream>
#include <fstream>

std::string Utilities::FileToString(std::string path)
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

std::string Utilities::PointerToString(void* pointer)
{
	return std::to_string(reinterpret_cast<std::uintptr_t>(pointer));
}

int Utilities::WrapIndex(int value, int arraySize)
{
	return (value % arraySize + arraySize) % arraySize;
}

float Utilities::Lerp(float a, float b, float t)
{
	return a * (1 - t) + b * t;
}

int Utilities::Lerp(int a, int b, float t)
{
	return a * (1 - t) + b * t;
}

glm::vec3 Utilities::Lerp(glm::vec3 a, glm::vec3 b, float t)
{
	return a * (1 - t) + b * t;
}

std::string Utilities::ToLower(std::string& str)
{
	std::string newString;
	for (char c : str)
	{
		newString += std::tolower(c);
	}
	return newString;
}

