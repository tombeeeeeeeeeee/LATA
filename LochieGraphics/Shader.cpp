#include "Shader.h"

#include "Utilities.h"

#include <iostream>
#include <sstream>
#include <fstream>

GLuint Shader::usingID = 0;

int Shader::getFlag()
{
	return updateFlag;
}

Shader::Shader(std::string _vertexPath, std::string _fragmentPath, int _updateFlag) :
	vertexPath(_vertexPath),
	fragmentPath(_fragmentPath),
	updateFlag(_updateFlag)
{
	Load();
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

void Shader::Load()
{
	if (loaded) { DeleteProgram(); }
	GLuint vertex = CompileShader(vertexPath, GL_VERTEX_SHADER);
	GLuint fragment = CompileShader(fragmentPath, GL_FRAGMENT_SHADER);

	GLID = glCreateProgram();
	glAttachShader(GLID, vertex);
	glAttachShader(GLID, fragment);
	glLinkProgram(GLID);

	// Print any linking errors
	int success;
	char infoLog[512];
	glGetProgramiv(GLID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(GLID, 512, NULL, infoLog);
		std::cout << "Shader error, program linking failed\n" << "Vertex path: " << vertexPath << "\n" << "Fragment path: " << fragmentPath << "\n" << infoLog << "\n";
	}
	else
	{
		std::cout << "Created shader ID: " << GLID << " with vertex: " << vertexPath << " and fragment: " << fragmentPath << "\n";
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);

	loaded = true;
}

GLint Shader::getUniformLocation(const std::string& name) const
{
	if (usingID != GLID) {
		std::cout << "Error: Using wrong shader when setting uniform\n";
	}
	GLint location = glGetUniformLocation(GLID, name.c_str());
	if (location == -1) {
		// TODO: Make a better error handling system instead of lagging and spamming the console
		std::cout << "Couldn't find: \"" << name << "\" in shader ID" << GLID << "\n";
	}
	return location;
}


void Shader::Use()
{
	if (usingID != GLID) {
		usingID = GLID;
		glUseProgram(GLID);
	}
}

void Shader::setBool(const std::string& name, bool value)
{
	glUniform1i(getUniformLocation(name), (int)value);
}

void Shader::setSampler(const std::string& name, GLuint value)
{
	glUniform1i(getUniformLocation(name), value);
}

void Shader::setInt(const std::string& name, int value)
{
	glUniform1i(getUniformLocation(name), value);
}

void Shader::setFloat(const std::string& name, float value)
{
	glUniform1f(getUniformLocation(name), value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value)
{
	glUniform2fv(getUniformLocation(name), 1, &value[0]);
}

void Shader::setVec2(const std::string& name, float x, float y)
{
	glUniform2f(getUniformLocation(name), x, y);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value)
{
	glUniform3fv(getUniformLocation(name), 1, &value[0]);
}

void Shader::setVec3(const std::string& name, float x, float y, float z)
{
	glUniform3f(getUniformLocation(name), x, y, z);
}

void Shader::setVec4(const std::string& name, const glm::vec4& value)
{
	glUniform4fv(getUniformLocation(name), 1, &value[0]);
}

void Shader::setVec4(const std::string& name, float x, float y, float z, float w)
{
	glUniform4f(getUniformLocation(name), x, y, z, w);
}

void Shader::setMat2(const std::string& name, const glm::mat2& mat)
{
	glUniformMatrix2fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat3(const std::string& name, const glm::mat3& mat)
{
	glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat)
{
	glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::DeleteProgram()
{
	glDeleteProgram(GLID);
}