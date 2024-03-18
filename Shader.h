#pragma once
#include <string>
#include "glm/mat4x4.hpp"+
#include "glad.h" // include glad to get the required OpenGL headers

class Shader
{
private:
	GLint getLocation(const std::string& name) const;
	static unsigned int CompileShader(const char* path, int type);
public:
	// the program ID
	unsigned int ID;
	// constructor reads and builds the shader
	Shader();
	Shader(const char* vertexPath, const char* fragmentPath);
	// use/activate the shader
	void Use();
	// utility uniform functions
	void setBool(const std::string& name, bool value) const;
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