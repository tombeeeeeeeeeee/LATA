#pragma once

#include <string>

class Image
{
public:
	unsigned char* data;

	std::string path;

	int width = 0;
	int height = 0;
	int components = 0;
	bool flipped = false;

	bool loaded = false;

	Image(std::string _path, bool _flipped = false, int _components = 0);
	Image(bool _flipped = false, int _components = 0);

	void Load();
	void Load(std::string path);

	unsigned char* getValueCompAt(int col, int row, int component = 0) const;

	~Image();
};

