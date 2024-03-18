#include "Shader.h"

#include <fstream>
#include <sstream>
#include <iostream>


Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	unsigned int vertex = CompileShader(vertexPath, GL_VERTEX_SHADER);
	unsigned int fragment = CompileShader(fragmentPath, GL_FRAGMENT_SHADER);

	// Shader Program
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);

	// Print any linking errors
	int success;
	char infoLog[512];
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "Shader error, program linking failed\n" << infoLog << "\n";
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

unsigned int Shader::CompileShader(const char* path, int type) 
{
	std::string code;
	std::ifstream file;

	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		// Open Files
		file.open(path);
		std::stringstream shaderStream;

		// Read files
		shaderStream << file.rdbuf();

		// Close files
		file.close();

		// Convert stream to string
		code = shaderStream.str();
	}
	catch (std::ifstream::failure e) {
		std::cout << "Shader error, file not read successfully\n";
	}
	const char* shaderCode = code.c_str();

	unsigned int shader;

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

GLint Shader::getLocation(const std::string& name) const
{
	GLint location = glGetUniformLocation(ID, name.c_str());
	if (location == -1) {
		std::cout << "Couldn't find \"" << name << "\" in shader ID" << ID << "\n";
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
	glUniform1i(getLocation(name), (int)value);
}

void Shader::setInt(const std::string& name, int value) const
{
	glUniform1i(getLocation(name), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
	glUniform1f(getLocation(name), value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
	glUniform2fv(getLocation(name), 1, &value[0]);
}

void Shader::setVec2(const std::string& name, float x, float y) const
{
	glUniform2f(getLocation(name), x, y);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
	glUniform3fv(getLocation(name), 1, &value[0]);
}

void Shader::setVec3(const std::string& name, float x, float y, float z) const
{
	glUniform3f(getLocation(name), x, y, z);
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
	glUniform4fv(getLocation(name), 1, &value[0]);
}

void Shader::setVec4(const std::string& name, float x, float y, float z, float w)
{
	glUniform4f(getLocation(name), x, y, z, w);
}

void Shader::setMat2(const std::string& name, const glm::mat2& mat) const
{
	glUniformMatrix2fv(getLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat3(const std::string& name, const glm::mat3& mat) const
{
	glUniformMatrix3fv(getLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const
{
	glUniformMatrix4fv(getLocation(name), 1, GL_FALSE, &mat[0][0]);
}


void Shader::DeleteProgram()
{
	glDeleteProgram(ID);
}
