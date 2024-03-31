#include "Material.h"

#include "Shader.h"

#include "Graphics.h"

Material::Material()
{
}

Material::Material(unsigned int textureCount, Texture* _textures)
{
	textures.reserve(textureCount);
	for (unsigned int i = 0; i < textureCount; i++)
	{
		textures.push_back(&_textures[i]);
	}
}

Material::Material(std::vector<Texture*> _textures)
{
	textures = _textures;
}

void Material::Use(Shader* shader)
{
	// Bind textures

	shader->Use();
	// Unbinds any extra assigned textures, this is so if a mesh only has one diffuse, the previously set specular from another mesh isn't used.
	//for (unsigned int i = textures.size(); i < textures.size() + 5; i++)
	for (unsigned int i = 0; i < 5; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i );
		glBindTexture(GL_TEXTURE_2D, 0);
	}


	std::unordered_map<Texture::Type, unsigned int> typeCounts;
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i + 1);
		unsigned int number;
		std::string name = Texture::TypeNames.find(textures[i]->type)->second;
		// Get the amount of this type of texture has been assigned
		auto typeCount = typeCounts.find(textures[i]->type);
		if (typeCount == typeCounts.end()) {
			//TODO: Should this be emplace or insert
			typeCount = typeCounts.emplace(std::pair<Texture::Type, unsigned int>(textures[i]->type, 1)).first;
		}
		else {
			typeCount->second++;
		}
		number = typeCount->second;

		// Set the sampler to the correct texture unit
		shader->setSampler(("material." + name + std::to_string(number)), i + 1);

		glBindTexture(GL_TEXTURE_2D, textures[i]->ID);
	}
}
