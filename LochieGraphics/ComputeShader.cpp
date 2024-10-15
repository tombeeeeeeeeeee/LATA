#include "ComputeShader.h"

ComputeShader::ComputeShader(std::string path)
{
	GLuint compute = Shader::CompileShader(path, GL_COMPUTE_SHADER);
	GLID = Shader::CreateProgram({ compute });
}

void ComputeShader::Run(unsigned int xGroups, unsigned int yGroups, unsigned int zGroups)
{
	Use();
	glDispatchCompute(xGroups, yGroups, zGroups);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}
