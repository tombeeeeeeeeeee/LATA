#include "ComputeShader.h"

ComputeShader::ComputeShader(std::string path)
{
	GLuint compute = Shader::CompileShader(path, GL_COMPUTE_SHADER);
	GLID = Shader::CreateProgram({ compute });
}
