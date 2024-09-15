#pragma once
#include "Maths.h"
#include <string>

namespace Utilities {
	std::string FileToString(std::string path);

	std::string PointerToString(void* pointer);

	int WrapIndex(int value, int arraySize);

	float Lerp(float a, float b, float t);
	int Lerp(int a, int b, float t);
	glm::vec2 Lerp(glm::vec2 a, glm::vec2 b, float t);
	glm::vec3 Lerp(glm::vec3 a, glm::vec3 b, float t);

	template <typename T> T mapValueTo(T value, T originalMin, T originalMax, T newMin, T newMax)
	{
		return (((value - originalMin) / (originalMax - originalMin)) * (newMax - newMin)) + newMin;
	}

	std::string ToLower(const std::string& str);

	std::string FilenameFromPath(const std::string& path, bool includeExtension = true);
}
