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
};

Texture::Texture(std::string _path, int _wrappingMode, bool flip) :
	path(_path),
	wrappingMode(_wrappingMode),
	flipped(flip)
{
	Load();
}

void Texture::Load()
{
	if (loaded) { DeleteTexture(); }
	stbi_set_flip_vertically_on_load(flipped);

	ID;
	glGenTextures(1, &ID);

	int width, height, components;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &components, STBI_default);
	if (data)
	{
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
			return;
			break;
		}

		glBindTexture(GL_TEXTURE_2D, ID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D); //TODO: Mip maps can look bad transitioning

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrappingMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrappingMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		std::cout << "Texture failed to load, path: " << path << "\n";
	}
	stbi_image_free(data);
	loaded = true;
}

GLuint Texture::LoadCubeMap(std::vector<std::string> faces)
{
	unsigned int ID;
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
	int width, height, components;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &components, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap failed to load at path: " << faces[i] << "\n";
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	return ID;
}

void Texture::UseCubeMap(GLuint ID, Shader* shader)
{
	shader->Use();
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
	shader->setSampler("cubeMap", 1);
}

void Texture::DeleteTexture()
{
	glDeleteTextures(1, &ID);
}
