#include "Shader.h"

#include "Paths.h"

#include "Utilities.h"

#include "Serialisation.h"
#include "Graphics.h"

#include <iostream>
#include <sstream>
#include <fstream>

GLuint Shader::usingID = 0;

int Shader::getFlag() const
{
	return updateFlag;
}

bool Shader::getFlag(int flag) const
{
	return (updateFlag & flag) == flag;
}

void Shader::AddFlag(int _flag)
{
	updateFlag |= _flag;
}

void Shader::RemoveFlag(int _flag)
{
	updateFlag &= ~_flag;
}

void Shader::setFlag(int _flag, bool state)
{
	if (state)
	{
		AddFlag(_flag);
	}
	else {
		RemoveFlag(_flag);
	}
}

Shader::Shader(std::string _vertexPath, std::string _fragmentPath, int _updateFlag) :
	vertexPath(_vertexPath),
	fragmentPath(_fragmentPath),
	updateFlag(_updateFlag)
{
	Load();
}

std::string Shader::GetShaderCode(std::string path)
{
	std::string code = Utilities::FileToString(path);

	return PreProcessShaderCode(code, path);
}

unsigned int Shader::CompileShaderFile(std::string path, int type)
{
	std::string code = GetShaderCode(path);
	return CompileShaderSource(code, type, path);
}

unsigned int Shader::CompileShaderSource(std::string code, int type, std::string debugInfo)
{
	std::string preProcessedCode = PreProcessShaderCode(code, Paths::importShaderLocation);
	const char* shaderCode = preProcessedCode.c_str();
	unsigned int shader = glCreateShader(type);
	glShaderSource(shader, 1, &shaderCode, nullptr);
	glCompileShader(shader);

	// Print any compile errors;
	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, nullptr, infoLog);
		std::cout << "Shader error, compilation failed, path: " << debugInfo << "\n" << infoLog << "\n";
	}
	return shader;
}

GLuint Shader::CreateProgram(std::vector<GLuint> shaders)
{
	auto GLID = glCreateProgram();
	for (auto& i : shaders)
	{
		glAttachShader(GLID, i);
	}
	glLinkProgram(GLID);

	// Print any linking errors
	int success;
	char infoLog[512];
	glGetProgramiv(GLID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(GLID, 512, NULL, infoLog);
		std::cout << "Shader error, program linking failed\n" << infoLog << "\n";
	}

	return GLID;
}

void Shader::Load()
{
	if (loaded) { DeleteProgram(); }
	GLuint vertex = CompileShaderFile(vertexPath, GL_VERTEX_SHADER);
	GLuint fragment = CompileShaderFile(fragmentPath, GL_FRAGMENT_SHADER);

	GLID = CreateProgram({ vertex, fragment });

	// TODO: Could keep track of individual vertex and fragment shaders for fast loading
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
		//std::cout << "Couldn't find: \"" << name << "\" in shader ID" << fragmentPath << "\n";
	}
	return location;
}

std::string Shader::PreProcessShaderCode(std::string code, std::string includePath)
{
	// Use this as the include symbol as thats what the vs extension very simply recognises
	static const std::string preInclude = "\n//! #include \"";

	for (size_t found = code.find(preInclude); found != std::string::npos; found = code.find(preInclude))
	{
		size_t endIncludeFileNameIndex = code.find_first_of('\"', found + preInclude.size());
		size_t startIncludeFileNameIndex = found + preInclude.size();
		std::string includeFileName = code.substr(startIncludeFileNameIndex, endIncludeFileNameIndex - startIncludeFileNameIndex);

		std::string pathWithoutFilename = Utilities::PathWithoutFilename(includePath);
		std::string includeFilePath = pathWithoutFilename + includeFileName;

		code.replace(found, endIncludeFileNameIndex + 1 - found, GetShaderCode(includeFilePath));
	}
	return code;
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

void Shader::setSampler(const std::string& name, unsigned int value)
{
	// TODO: This might should be glUniform1ui, u for unsigned, check
	glUniform1i(getUniformLocation(name), value);
}

void Shader::setInt(const std::string& name, int value)
{
	glUniform1i(getUniformLocation(name), value);
}

void Shader::setIntArray(const std::string& name, const int* values, int count)
{
	glUniform1iv(getUniformLocation(name), count, values);
}

void Shader::setIVec2(const std::string& name, const glm::ivec2& value)
{
	glUniform2iv(getUniformLocation(name), 1, &value[0]);
}

void Shader::setUnsignedInt(const std::string& name, unsigned int value)
{
	glUniform1ui(getUniformLocation(name), value);
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

Shader::operator std::string() const
{
	return getDisplayName();
}

std::string Shader::getDisplayName() const
{
	// TODO: Better name
	return fragmentPath + " " + std::to_string(GUID);
}

toml::table Shader::Serialise() const
{
	return toml::table{
		{ "guid", Serialisation::SaveAsUnsignedLongLong(GUID) },
		{ "vertex", vertexPath },
		{ "fragment", fragmentPath },
		{ "flags", updateFlag }
	};
}

Shader::Shader(toml::table toml) : Shader(&toml)
{
	// This isn't empty, uses other constructor ^
}

Shader::Shader(toml::table* toml)
{
	auto temp = (*toml)["guid"];
	GUID = Serialisation::LoadAsUnsignedLongLong((*toml)["guid"]);
	vertexPath = Serialisation::LoadAsString((*toml)["vertex"]);
	fragmentPath = Serialisation::LoadAsString((*toml)["fragment"]);
	updateFlag = Serialisation::LoadAsInt((*toml)["flags"]);
	
	Load();
}

void Shader::SaveAsAsset() const
{
	std::ofstream file(Paths::shadersSaveLocation + Utilities::FilenameFromPath(fragmentPath, false) + Paths::shaderExtension);
	file << Serialise();
	file.close();

}
