#pragma once

#include "Texture.h"

#include "Graphics.h"

class FrameBuffer
{
public:
	GLuint FBO;
	unsigned int width;
	unsigned int height;
	// TODO:
	// Texture* texture;
	GLuint colourTexID;
	GLuint depthTextureID;

	FrameBuffer(unsigned int _width, unsigned int _height, GLuint _colourTexID, GLuint _depthTexID);

	void Bind();

	static void Unbind();

	~FrameBuffer();
};

