#pragma once

#include "Graphics.h"

#include "glm/mat4x4.hpp"

#include <string>

class Shader
{
private:
	GLint getUniformLocation(const std::string& name) const;
	static GLuint CompileShader(std::string path, int type);
public:
	// the program ID
	GLuint ID;
	Shader();
	// TODO: Contructor from one name that finds all the shaders of different types
	Shader(std::string vertexPath, std::string fragmentPath);
	static GLuint Load(std::string vertexPath, std::string fragmentPath);
	// use/activate the shader
	void Use();
	// Uniform functions
	//TODO: these can be called without any warning when the shader program is not being used, it should either use the shader or send message maybe
	void setBool(const std::string& name, bool value) const;
	void setSampler(const std::string& name, GLuint value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setVec2(const std::string& name, const glm::vec2& value) const;
	void setVec2(const std::string& name, float x, float y) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec3(const std::string& name, float x, float y, float z) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;
    void setVec4(const std::string& name, float x, float y, float z, float w);
	void setMat2(const std::string& name, const glm::mat2& mat) const;
	void setMat3(const std::string& name, const glm::mat3& mat) const;
	void setMat4(const std::string& name, const glm::mat4& mat) const;

	void DeleteProgram();
};