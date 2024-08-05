#pragma once

#include <string>

namespace Utilities {
	std::string FileToString(std::string path);

	std::string PointerToString(void* pointer);

	int WrapIndex(int value, int arraySize);
}
