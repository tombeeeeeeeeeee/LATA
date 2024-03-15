#pragma once
class Texture
{
public:
	unsigned int ID;
	bool initialised = false;

	Texture(const char* path);
	Texture();
	void Initialise(const char* path);
	void Delete();

	~Texture();

private:
	static unsigned int LoadTexture(const char* path);
};

