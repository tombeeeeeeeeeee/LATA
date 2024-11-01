#include "ComputeShader.h"

#include "Graphics.h"

ComputeShader::ComputeShader(std::string path)
{
	GLuint compute = Shader::CompileShader(path, GL_COMPUTE_SHADER);
	GLID = Shader::CreateProgram({ compute });
}

void ComputeShader::Run(unsigned int xGroups, unsigned int yGroups, unsigned int zGroups)
{
	Use();
	glDispatchCompute(xGroups, yGroups, zGroups);
	// TODO: This isn't always going to be the right memory barrier
	// TODO: Move to before draw instead of here
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	glFinish();
	//glFlush();
}
