#pragma once

#include <string>

std::string FileToString(std::string path);

std::string PointerToString(void* pointer);

namespace Utilities {
	int Wrap(int value, int arraySize);
}
