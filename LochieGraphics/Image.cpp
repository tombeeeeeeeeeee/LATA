#include "Image.h"

#include "stb_image.h"

#include <iostream>

Image::Image(std::string _path, bool _flipped) :
	path(path),
	flipped(_flipped)
{
	Load();
}

Image::Image(bool _flipped) :
	path(""),
	flipped(_flipped)
{
}

void Image::Load()
{
	if (path != "") {
		stbi_set_flip_vertically_on_load(flipped);
		data = stbi_load(path.c_str(), &width, &height, &components, STBI_default);
	}
	else {
		std::cout << "Cannot load image, no path given\n";
	}
}

unsigned char* Image::getValueCompAt(int col, int row, int component)
{
	int i = (col + width * row) * components + component;
	return data + i;
}

Image::~Image()
{
	// TODO:
}
