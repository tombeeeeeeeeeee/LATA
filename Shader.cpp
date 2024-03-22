#include "Shader.h"

#include "Utilities.h"

#include <iostream>
#include <sstream>
#include <fstream>

Shader::Shader(std::string vertexPath, std::string fragmentPath)
{
	// Shader Program
	ID = Load(vertexPath, fragmentPath);
}

GLuint Shader::CompileShader(std::string path, int type)
{
	std::string temp = FileToString(path);
	const char* shaderCode = temp.c_str();

	GLuint shader;

	int success;
	char infoLog[512];

	shader = glCreateShader(type);
	glShaderSource(shader, 1, &shaderCode, nullptr);
	glCompileShader(shader);

	// Print any compile errors;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, 512, nullptr, infoLog);
		std::cout << "Shader error, compilation failed\n" << infoLog << "\n";
	}
	return shader;
}

GLuint Shader::Load(std::string vertexPath, std::string fragmentPath)
{
	GLuint vertex = CompileShader(vertexPath, GL_VERTEX_SHADER);
	GLuint fragment = CompileShader(fragmentPath, GL_FRAGMENT_SHADER);

	GLuint ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);

	// Print any linking errors
	int success;
	char infoLog[512];
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "Shader error, program linking failed\n" << "Vertex path: " << vertexPath << "\n" << "Fragment path: " << fragmentPath << "\n" << infoLog << "\n";
	}
	else
	{
		std::cout << "Created shader ID: " << ID << " with vertex: " << vertexPath << " and fragment: " << fragmentPath << "\n";
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);

	return ID;
}

GLint Shader::getUniformLocation(const std::string& name) const
{
	GLint location = glGetUniformLocation(ID, name.c_str());
	if (location == -1) {
		std::cout << "Couldn't find: \"" << name << "\" in shader ID" << ID << "\n";
	}
	return location;
}


Shader::Shader() : ID(-1)
{
}

void Shader::Use()
{
	glUseProgram(ID);
}

void Shader::setBool(const std::string& name, bool value) const
{
	glUniform1i(getUniformLocation(name), (int)value);
}

void Shader::setSampler(const std::string& name, GLuint value) const
{
	glUniform1i(getUniformLocation(name), value);
}

void Shader::setInt(const std::string& name, int value) const
{
	glUniform1i(getUniformLocation(name), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
	glUniform1f(getUniformLocation(name), value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
	glUniform2fv(getUniformLocation(name), 1, &value[0]);
}

void Shader::setVec2(const std::string& name, float x, float y) const
{
	glUniform2f(getUniformLocation(name), x, y);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
	glUniform3fv(getUniformLocation(name), 1, &value[0]);
}

void Shader::setVec3(const std::string& name, float x, float y, float z) const
{
	glUniform3f(getUniformLocation(name), x, y, z);
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
	glUniform4fv(getUniformLocation(name), 1, &value[0]);
}

void Shader::setVec4(const std::string& name, float x, float y, float z, float w)
{
	glUniform4f(getUniformLocation(name), x, y, z, w);
}

void Shader::setMat2(const std::string& name, const glm::mat2& mat) const
{
	glUniformMatrix2fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat3(const std::string& name, const glm::mat3& mat) const
{
	glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const
{
	glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::DeleteProgram()
{
	glDeleteProgram(ID);
}