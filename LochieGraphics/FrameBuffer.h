#pragma once

#include "Texture.h"

#include "Graphics.h"

class FrameBuffer
{
public:
	GLuint FBO;
	unsigned int width;
	unsigned int height;
	Texture* colourTexture;
	Texture* depthTexture;

	FrameBuffer(unsigned int _width, unsigned int _height, Texture* _colourTexture, Texture* _depthTexture);

	void RefreshTextures();

	void Bind();

	static void Unbind();

	~FrameBuffer();
};

