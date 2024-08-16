#include "Image.h"

#include "stb_image.h"

#include <iostream>

Image::Image(std::string _path, bool _flipped, int _components) :
	path(path),
	flipped(_flipped),
	components(_components)
{
	Load();
}

Image::Image(bool _flipped, int _components) :
	path(""),
	flipped(_flipped),
	components(_components)
{
}

void Image::Load()
{
	if (path != "") {
		stbi_set_flip_vertically_on_load(flipped);
		int actualComponentsInFile;
		data = stbi_load(path.c_str(), &width, &height, &actualComponentsInFile, components);
		loaded = true;
	}
	else {
		std::cout << "Cannot load image, no path given\n";
	}
}

void Image::Load(std::string _path)
{
	path = _path;
	Load();
}

unsigned char* Image::getValueCompAt(int col, int row, int component) const
{
	int i = (col + width * row) * components + component;
	return data + i;
}

Image::~Image()
{

	// TODO:
}
