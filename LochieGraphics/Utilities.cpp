#include "Utilities.h"

#include <iostream>
#include <sstream>
#include <fstream>

std::string Utilities::FileToString(std::string path)
{
	std::stringstream output;
	std::ifstream file(path);

	if (!file.is_open())
	{
		std::cout << "Error loading file: " << path << std::endl;
		return "";
	}
	while (!file.eof())
	{
		std::string thisLine;
		std::getline(file, thisLine);
		output << thisLine << "\n";
	}
	return output.str().substr(0, output.str().size() - 1);
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
	return (int)(a * (1 - t) + b * t);
}

glm::vec2 Utilities::Lerp(glm::vec2 a, glm::vec2 b, float t)
{
	return a * (1 - t) + b * t;
}

glm::vec3 Utilities::Lerp(glm::vec3 a, glm::vec3 b, float t)
{
	return a * (1 - t) + b * t;
}

glm::vec2 Utilities::ClampMag(glm::vec2 v, float min, float max)
{
	float originalLength = glm::length(v);
	glm::vec2 normalized = glm::normalize(v);
	if (originalLength < min) originalLength = min;
	else if (originalLength > max) originalLength = max;
	return normalized * originalLength;
}

glm::vec3 Utilities::ClampMag(glm::vec3 v, float min, float max)
{
	float originalLength = glm::length(v);
	glm::vec3 normalized = glm::normalize(v);
	if (originalLength < min) originalLength = min;
	else if (originalLength > max) originalLength = max;
	return normalized * originalLength;
}

std::string Utilities::ToLower(const std::string& str)
{
	std::string newString;
	for (char c : str)
	{
		newString += std::tolower(c);
	}
	return newString;
}

std::string Utilities::FilenameFromPath(const std::string& path, bool includeExtension)
{
	unsigned long long start = path.find_last_of("/\\") + 1;
	if (includeExtension) {
		return path.substr(start);
	}
	return path.substr(start, path.find_last_of('.') - start);
}

