#include "Texture.h"

#include "Shader.h"

#include "Graphics.h"

#include "stb_image.h"

#include <iostream>

const std::unordered_map<Texture::Type, std::string> Texture::TypeNames = 
{
	{ Type::diffuse, "diffuse" },
	{ Type::specular, "specular" },
	{ Type::normal, "normal" },
	{ Type::height, "height" },
	{ Type::emission, "emission" }, // TODO: Should cubemap be a type?
	{ Type::albedo, "albedo" },
	{ Type::metallic, "metallic" },
	{ Type::roughness, "roughness" },
	{ Type::ao, "ao" },
};

Texture::Texture(std::string _path, Type _type, int _wrappingMode, bool flip) :
	path(_path),
	type(_type),
	wrappingMode(_wrappingMode),
	flipped(flip)
{
	Load();
}

void Texture::Load()
{
	if (loaded) { DeleteTexture(); }
	stbi_set_flip_vertically_on_load(flipped);

	int width, height, components;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &components, STBI_default);
	if (!data) {
		std::cout << "Texture failed to load, path: " << path << "\n";
		return;
	}

	GLenum format;
	switch (components)
	{
	case STBI_grey:
		format = GL_RED;
		break;
	case STBI_grey_alpha:
		format = GL_RG;
		break;
	case STBI_rgb:
		format = GL_RGB;
		break;
	case STBI_rgb_alpha:
		format = GL_RGBA;
		break;
	default:
		std::cout << "Texture failed to load, could not be read correctly, path: " << path << "\n";
		stbi_image_free(data);
		return; // Return instead
	}

	GLID = CreateTexture(width, height, format, data, wrappingMode, GL_UNSIGNED_BYTE, true, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
	stbi_image_free(data);
	loaded = true;
}

Texture::Texture(unsigned int width, unsigned int height, GLenum format, unsigned char* data, GLint wrappingMode, GLenum dataType, bool mipMaps, GLint minFilter, GLint magFilter)
{
	if (loaded) { DeleteTexture(); }
	GLID = CreateTexture(width, height, format, data, wrappingMode, dataType, mipMaps, minFilter, magFilter);
	loaded = true;
}

GLuint Texture::CreateTexture(unsigned int width, unsigned int height, GLenum format, unsigned char* data, GLint wrappingMode, GLenum dataType, bool mipMaps, GLint minFilter, GLint magFilter)
{
	GLuint ID;
	glGenTextures(1, &ID);

	glBindTexture(GL_TEXTURE_2D, ID);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, dataType, data);
	
	if (mipMaps) {
		glGenerateMipmap(GL_TEXTURE_2D); //TODO: Mip maps can look bad transitioning
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrappingMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrappingMode);

	return ID;
}

GLuint Texture::LoadCubeMap(std::string faces[6])
{
	unsigned int GLID;
	glGenTextures(1, &GLID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, GLID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	int width, height, components;
	for (unsigned int i = 0; i < 6; i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &components, 0);
		if (!data) {
			std::cout << "Cubemap face failed to load at path: " << faces[i] << "\n";
			continue;
		}
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
	}
	return GLID;
}

void Texture::UseCubeMap(GLuint GLID, Shader* shader)
{
	shader->Use();
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, GLID);
	shader->setSampler("cubeMap", 1);
}

void Texture::DeleteTexture()
{
	glDeleteTextures(1, &GLID);
}
