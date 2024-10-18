#pragma once

#include "Graphics.h"

#include "assimp/scene.h"
#include "assimp/cimport.h"

#include <string>
#include <unordered_map>

class Shader;
namespace toml {
	inline namespace v3 {
		class table;
	}
}

// TODO: I think textures should be used slightly differently with materials, rather than always binding them starting from 1, as many textures should be bound at once and only if there is more than the limit (32/31?) should it actually be changed 
// The limit isn't 32, there is a gl function to get the limit
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
		roughness,
		metallic,
		ao,
		PBR,
		paint,
		count // When adding types ensure to add the type name to the unordered map below
	};
	static const std::unordered_map<Type, std::string> TypeNames;
	

	unsigned long long GUID;
	GLuint GLID;

	std::string path = "";
	int width;
	int height;
	Type type = Type::diffuse;
	GLenum wrappingMode;
	GLenum format;
	GLenum dataType;
	bool mipMapped;
	GLint minFilter;
	GLint maxFilter;
	bool flipped; //TODO: rename to flip?

	bool loaded = false;

	Texture(std::string _path, Type _type, int _wrappingMode, bool flip = false);

	void Load(unsigned char* data = nullptr);

	Texture(int _width, int _height, GLenum _format, unsigned char* _data, GLint _wrappingMode, GLenum _dataType, bool _mipMaps, GLint _minFilter, GLint _magFilter);
	static GLuint CreateTexture(int width, int height, GLenum format, unsigned char* data, GLint wrappingMode, GLenum dataType, bool mipMaps, GLint minFilter, GLint magFilter);
	void setWidthHeight(int _width, int _height);

	// TODO: The way textures are bound should be re considered
	// TODO: This could take nothing and return the index it is assigned to instead
	void Bind(int indexToBindTo);
	static void ClearBind(int indexToClearBind);

	/// <summary>
	/// Given 6 faces, a cube map textured will be generated and returned
	/// </summary>
	/// <param name="faces">Expected in the order of: Right, Left, Top, Bottom, Front, Back</param>
	/// <returns>The GLuint index of the cube map texture</returns>
	static GLuint LoadCubeMap(std::string faces[6]);
	static void UseCubeMap(GLuint GLID, Shader* shader);

	void DeleteTexture();

	static void EditorGUI(std::vector<Texture*>& textures);

	operator std::string() const;
	std::string getDisplayName() const;
	static std::string getTypelessFilename(std::string path);
	std::string getTypelessFilename() const;
	std::string getSaveName() const;
	static std::string getSaveName(std::string path, Type type);

	void OpenModal() {};

	toml::table Serialise() const;
	Texture(toml::table table);
	void SaveAsAsset() const;

private:

	Texture() = default;
};

