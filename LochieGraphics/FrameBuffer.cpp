#include "FrameBuffer.h"

FrameBuffer::FrameBuffer(unsigned int _width, unsigned int _height, Texture* _colourTexture, Texture* _depthTexture) :
	width(_width),
	height(_height),
	colourTexture(_colourTexture),
	depthTexture(_depthTexture)
{
	glGenFramebuffers(1, &FBO);
	RefreshTextures();
}

void FrameBuffer::RefreshTextures()
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
}
