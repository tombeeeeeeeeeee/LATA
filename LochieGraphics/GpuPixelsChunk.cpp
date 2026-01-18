#include "GpuPixelsChunk.h"

#include "Graphics.h"

void PixelsGPU::Chunk::BindSSBO(int index) const
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, ssbo1);
}

void PixelsGPU::Chunk::SwitchReadWriteSSBOs()
{
	readSsboFirst = !readSsboFirst;
}

void PixelsGPU::Chunk::Initialise()
{
	glGenBuffers(1, &ssbo1);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo1);

	glBufferData(GL_SHADER_STORAGE_BUFFER, CalculateSsboSize(), nullptr, GL_STATIC_COPY);
	glClearNamedBufferData(ssbo1, GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, ssbo1);
	readSsboFirst = true;
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void PixelsGPU::Chunk::Deinitalise()
{
	glDeleteBuffers(1, &ssbo1);
}

PixelsGPU::Chunk::Chunk(glm::ivec2 _coords) :
	coords(_coords)
{
}

PixelsGPU::Chunk::~Chunk()
{
}
