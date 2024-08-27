#include "FrameBuffer.h"

#include "Texture.h"

#include <iostream>

FrameBuffer::FrameBuffer(unsigned int _width, unsigned int _height, Texture* _colourTexture, Texture* _depthTexture, bool _useRBO) :
	width(_width),
	height(_height),
	colourTexture(_colourTexture),
	depthTexture(_depthTexture),
	useRBO(_useRBO)
{
	glGenFramebuffers(1, &FBO);
	Load();
}

void FrameBuffer::setWidthHeight(unsigned int _width, unsigned int _height)
{
	width = _width;
	height = _height;
	Load();
}

void FrameBuffer::Load()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	if (depthTexture) {
		// TODO: glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTextureID, 0)
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture->GLID, 0);

		if (!colourTexture) {
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		}
	}

	if (colourTexture) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colourTexture->GLID, 0);
	}

	if (useRBO) {
		glGenRenderbuffers(1, &RBO);
		glBindRenderbuffer(GL_RENDERBUFFER, RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
	}


	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Error: Framebuffer is not complete!" << "\n";
	}

	Unbind();
}

void FrameBuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}

void FrameBuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &FBO);
	if (useRBO) {
		glDeleteRenderbuffers(1, &RBO);
	}
}
