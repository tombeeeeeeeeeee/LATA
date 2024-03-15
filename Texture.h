#pragma once
class Texture
{
	unsigned int ID;
	bool initialised = false;

	Texture(const char* path);
	Texture();
	void Initialise(const char* path);

	~Texture();

private:
	static unsigned int LoadTexture(const char* path);
};

