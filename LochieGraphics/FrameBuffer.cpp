#include "FrameBuffer.h"

FrameBuffer::FrameBuffer(unsigned int _width, unsigned int _height, GLuint _colourTexID, GLuint _depthTexID) :
	width(_width),
	height(_height),
	colourTexID(_colourTexID),
	depthTextureID(_depthTexID)
{
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	if (depthTextureID) {
		// TODO: glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTextureID, 0)
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTextureID, 0);

		if (!colourTexID) {
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		}
	}

	if (colourTexID) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, depthTextureID, 0);
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
