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
		emission
	};
	static const std::unordered_map<Type, std::string> TypeNames;

	GLuint ID;
	Type type;
	Texture();

	static GLuint Load(std::string path, int wrappingMode);

	static GLuint LoadCubeMap(std::vector<std::string> faces);
	static void UseCubeMap(GLuint ID, Shader* shader);

	void DeleteTexture();
};

