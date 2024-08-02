#pragma once

#include <string>

namespace Utilities {
	std::string FileToString(std::string path);

	std::string PointerToString(void* pointer);

	int Wrap(int value, int arraySize);
}
