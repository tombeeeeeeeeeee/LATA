#pragma once

#include <string>

std::string FileToString(std::string path);

std::string PointerToString(void* pointer);

int Wrap(int value, int arraySize)
{
	return (value % arraySize + arraySize) % arraySize;
}
