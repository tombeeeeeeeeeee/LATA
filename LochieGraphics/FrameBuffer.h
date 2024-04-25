#pragma once

#include "Texture.h"

#include "Graphics.h"

class FrameBuffer
{
private:
	GLuint FBO;
	GLuint RBO;
	bool useRBO;
	unsigned int width;
	unsigned int height;
	Texture* colourTexture;
	Texture* depthTexture;

public:
	FrameBuffer(unsigned int _width, unsigned int _height, Texture* _colourTexture, Texture* _depthTexture, bool _useRBO);
	
	void setWidthHeight(unsigned int _width, unsigned int _height);

	void Load();

	void Bind();

	static void Unbind();

	~FrameBuffer();
};

