#pragma once

#include "Graphics.h"

class Texture;

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
	// Width and height is only needed if an rbo is being used, adjust constructor/s
	FrameBuffer(unsigned int _width, unsigned int _height, Texture* _colourTexture, Texture* _depthTexture, bool _useRBO);
	
	void setWidthHeight(unsigned int _width, unsigned int _height);

	void Load();

	// TODO: Lochie, change this to like push pop, keep a stack of the framebuffers
	void Bind();

	void Unbind();

	~FrameBuffer();
};

