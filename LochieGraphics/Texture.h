#pragma once

#include "Graphics.h"

#include "assimp/scene.h"
#include "assimp/cimport.h"

#include <string>
#include <unordered_map>

class Shader;

// TODO: I think textures should be used slightly differently with materials, rather than always binding them starting from 1, as many textures should be bound at once and only if there is more than the limit (32/31?) should it actually be changed 
class Texture
{
public:
	// TODO: Reconsider if textures need keep its type
	enum class Type {
		diffuse,
		specular,
		normal,
		height,
		emission,
		albedo,
		metallic,
		roughness,
		ao,
		count // When adding types ensure to add the type name to the unordered map below
	};
	static const std::unordered_map<Type, std::string> TypeNames;

	unsigned long long GUID;
	GLuint GLID;
	Type type = Type::diffuse;
	std::string path = "";
	GLenum wrappingMode;
	bool flipped; //TODO: rename to flip?
	bool loaded = false;
	Texture(std::string _path, Type _type, int _wrappingMode, bool flip = false);

	void Load();

	Texture(unsigned int width = 1024, unsigned int height = 1024, GLenum format = GL_RGBA, unsigned char* data = nullptr, GLint wrappingMode = GL_REPEAT, GLenum dataType = GL_UNSIGNED_BYTE, bool mipMaps = false, GLint minFilter = GL_LINEAR, GLint magFilter = GL_LINEAR);
	static GLuint CreateTexture(unsigned int width = 1024, unsigned int height = 1024, GLenum format = GL_RGBA, unsigned char* data = nullptr, GLint wrappingMode = GL_REPEAT, GLenum dataType = GL_UNSIGNED_BYTE, bool mipMaps = false, GLint minFilter = GL_LINEAR, GLint magFilter = GL_LINEAR);

	/// <summary>
	/// Given 6 faces, a cube map textured will be generated and returned
	/// </summary>
	/// <param name="faces">Expected in the order of: Right, Left, Top, Bottom, Front, Back</param>
	/// <returns>The GLuint index of the cube map texture</returns>
	static GLuint LoadCubeMap(std::string faces[6]);
	static void UseCubeMap(GLuint GLID, Shader* shader);

	void DeleteTexture();

private:

	Texture() = default;
};

