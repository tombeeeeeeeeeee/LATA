#pragma once

#include "Graphics.h"

#include "assimp/scene.h"
#include "assimp/cimport.h"

#include <string>
#include <unordered_map>

class Shader;

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

	static GLuint LoadCubeMap(std::vector<std::string> faces);
	static void UseCubeMap(GLuint GLID, Shader* shader);

	void DeleteTexture();
};

