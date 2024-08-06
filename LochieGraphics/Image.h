#pragma once

#include <string>

class Image
{
public:
	unsigned char* data;

	std::string path;

	int width;
	int height;
	int components;
	bool flipped = false;

	bool loaded = false;

	Image(std::string _path, bool _flipped = false);
	Image(bool _flipped = false);

	void Load();

	unsigned char* getValueCompAt(int col, int row, int component = 0);


	~Image();
};

