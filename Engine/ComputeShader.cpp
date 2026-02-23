#include "ComputeShader.h"

#include "Graphics.h"

ComputeShader::ComputeShader(std::string path)
{
	GLuint compute = Shader::CompileShaderFile(path, GL_COMPUTE_SHADER);
	GLID = Shader::CreateProgram({ compute });
}

ComputeShader* ComputeShader::CreateCustomComputeShader(std::string code)
{
	GLuint computeProgram = Shader::CompileShaderSource(code, GL_COMPUTE_SHADER);
	ComputeShader* computeShader = new ComputeShader();
	computeShader->GLID = Shader::CreateProgram({ computeProgram });
	return computeShader;
}

void ComputeShader::Run(unsigned int xGroups, unsigned int yGroups, unsigned int zGroups, unsigned int barrier)
{
	Use();
	glDispatchCompute(xGroups, yGroups, zGroups);
	if (barrier != 0)
	{
		glMemoryBarrier(barrier);
	}
}
