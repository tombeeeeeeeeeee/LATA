#pragma once

#include "Maths.h"

#include <string>
#include <vector>

namespace toml {
	inline namespace v3 {
		class table;
	}
}

// TODO: Some shaders can be quite similar, see if there is or make a solution to not having to have to have a lot of shaders.
// Shader system
// Need some sort of #include system, theres a lot of functions that would be better off to be in a single file and copied over
// Structs
// 'Static' functions
// 
// There seems to be an extension for opengl already https://registry.khronos.org/OpenGL/extensions/ARB/ARB_shading_language_include.txt
// Is it worth using that or doing it myself, 
// note: recursion


class Shader
{
protected:
	int getUniformLocation(const std::string& name) const;
	static unsigned int CompileShader(std::string path, int type);
	static unsigned int CreateProgram(std::vector<unsigned int> shaders);
	bool loaded = false;
	int updateFlag;

	Shader() = default;

public:

	enum Flags {
		None     = 0,
		VPmatrix = 1 << 0,
		Lit      = 1 << 1,
		Animated = 1 << 2,
		Spec	 = 1 << 3,
		Painted  = 1 << 4,
	};

	int getFlag() const;
	bool getFlag(int _flag) const;
	void AddFlag(int _flag);
	void RemoveFlag(int _flag);
	void setFlag(int _flag, bool state);
	static unsigned int usingID;
	// the program ID
	unsigned int GLID;
	unsigned long long GUID = 0;
	Shader(std::string vertexPath, std::string fragmentPath, int _updateFlag = 0);
	void Load();
	std::string vertexPath;
	std::string fragmentPath;
	// use/activate the shader
	void Use();

	// Uniform functions
	//TODO: these can be called without any warning when the shader program is not being used, it should either use the shader or send message maybe
	// Although these can be set as const, it does not feel right and I will not be
	void setBool(const std::string& name, bool value);
	void setSampler(const std::string& name, unsigned int value);
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

	operator std::string() const;
	std::string getDisplayName() const;

	toml::table Serialise() const;
	Shader(toml::table toml);
	Shader(toml::table* toml);

	void SaveAsAsset() const;

	void OpenModal() {};

};