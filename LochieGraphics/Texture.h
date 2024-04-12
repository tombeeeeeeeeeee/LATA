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
		count
	};
	static const std::unordered_map<Type, std::string> TypeNames;

	unsigned long long GUID;
	GLuint GLID;
	Type type;
	std::string path;
	int wrappingMode;
	bool flipped; //TODO: rename to flip?
	bool loaded = false;
	Texture(std::string _path, Type _type, int _wrappingMode, bool flip = false);

	void Load();

	/// <summary>
	/// Given 6 faces, a cube map textured will be generated and returned
	/// </summary>
	/// <param name="faces">Expected in the order of: Right, Left, Top, Bottom, Front, Back</param>
	/// <returns>The GLuint index of the cube map texture</returns>
	static GLuint LoadCubeMap(std::string faces[6]);
	static void UseCubeMap(GLuint GLID, Shader* shader);

	void DeleteTexture();
};

