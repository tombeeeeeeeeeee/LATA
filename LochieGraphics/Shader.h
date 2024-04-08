#pragma once

#include "Graphics.h"

#include "Maths.h"

#include <string>


// TODO: Some shaders can be quite similar, see if there is or make a solution to not having to have to have a lot of shaders.
// Shader system
// Need some sort of #include system, theres a lot of functions that would be better off to be in a single file and copied over
// Structs
// 'Static' functions
// 
// 
// There seems to be an extension for opengl already https://registry.khronos.org/OpenGL/extensions/ARB/ARB_shading_language_include.txt
// Is it worth using that or doing it myself, 
// note: recursion
//

class Shader
{
private:
	GLint getUniformLocation(const std::string& name) const;
	static GLuint CompileShader(std::string path, int type);
	bool loaded = false;
public:
	static GLuint currentID;
	// the program ID
	GLuint GLID;
	// TODO: Contructor from one name that finds all the shaders of different types
	Shader(std::string vertexPath, std::string fragmentPath);
	void Load();
	std::string vertexPath;
	std::string fragmentPath;
	// use/activate the shader
	void Use();
	// Uniform functions
	//TODO: these can be called without any warning when the shader program is not being used, it should either use the shader or send message maybe
	// TODO: These can be const and have seen them as such, should they?
	void setBool(const std::string& name, bool value);
	void setSampler(const std::string& name, GLuint value);
	void setInt(const std::string& name, int value);
	void setFloat(const std::string& name, float value);
	void setVec2(const std::string& name, const glm::vec2& value);
	void setVec2(const std::string& name, float x, float y);
    void setVec3(const std::string& name, const glm::vec3& value);
    void setVec3(const std::string& name, float x, float y, float z);
    void setVec4(const std::string& name, const glm::vec4& value);
    void setVec4(const std::string& name, float x, float y, float z, float w);
	void setMat2(const std::string& name, const glm::mat2& mat);
	void setMat3(const std::string& name, const glm::mat3& mat);
	void setMat4(const std::string& name, const glm::mat4& mat);

	void DeleteProgram();
};