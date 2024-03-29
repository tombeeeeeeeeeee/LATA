#include "ResourceManager.h"

#include "stb_image.h"
#include "imguiStuff.h"

#include <iostream>

std::unordered_map<std::string, Texture, ResourceManager::hashFNV1A> ResourceManager::textures;
std::unordered_map<std::string, Shader, ResourceManager::hashFNV1A> ResourceManager::shaders;
std::unordered_map<std::vector<Texture*>, Material, ResourceManager::hashFNV1A> ResourceManager::materials;


const unsigned long long ResourceManager::hashFNV1A::offset = 14695981039346656037;
const unsigned long long ResourceManager::hashFNV1A::prime = 1099511628211;


Texture* ResourceManager::GetTexture(std::string path, Texture::Type type, int wrappingMode, bool flipOnLoad)
{
	stbi_set_flip_vertically_on_load(flipOnLoad);
	auto texture = textures.find(path);

	if (texture == textures.end()) {
		Texture newTexture;
		newTexture.ID = Texture::Load(path.c_str(), wrappingMode);
		newTexture.type = type;

		texture = textures.emplace(path, newTexture).first;
	}

	return &texture->second;
}

Shader* ResourceManager::GetShader(std::string vertexPath, std::string fragmentPath)
{
	std::string path = vertexPath + fragmentPath;
	auto shader = shaders.find(path);

	if (shader == shaders.end()) {
		Shader newShader;
		newShader.ID = Shader::Load(vertexPath, fragmentPath);

		shader = shaders.emplace(path, newShader).first;
	}

	return &shader->second;
}

//TODO: Sort textures in a specific order so that it doesn't make a new texture for the same material if the textures are given in a different order
Material* ResourceManager::GetMaterial(std::vector<Texture*> textures)
{
	auto material = materials.find(textures);

	if (material == materials.end()) {
		Material newMaterial;
		newMaterial.textures = textures;

		material = materials.emplace(textures, newMaterial).first;
	}

	return &material->second;
}

unsigned long long ResourceManager::hashFNV1A::operator()(std::string key) const
{
	unsigned long long hash = offset;
	for (auto i = 0; i < key.size(); i++)
	{
		hash ^= key[i];
		hash *= prime;
	}
	return hash;
}

unsigned long long ResourceManager::hashFNV1A::operator()(std::vector<Texture*> key) const
{
	unsigned long long hash = offset;
	for (auto t = key.begin(); t != key.end(); t++)
	{
		hash ^= (*t)->ID;
		hash *= prime;
	}
	return hash;
}

ResourceManager::~ResourceManager()
{
	UnloadAll();
}

void ResourceManager::GUI()
{
	if (ImGui::BeginTable("Resource Textures", 3)) {
		ImGui::TableNextRow();

		ImGui::TableSetColumnIndex(0);
		ImGui::Text("ID");

		ImGui::TableSetColumnIndex(1);
		ImGui::Text("Path");

		ImGui::TableSetColumnIndex(2);
		ImGui::Text("Type");

		for (auto i = ResourceManager::textures.begin(); i != ResourceManager::textures.end(); i++)
		{
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::Text(std::to_string(i->second.ID).c_str());

			ImGui::TableSetColumnIndex(1);
			ImGui::Text(i->first.c_str());

			ImGui::TableSetColumnIndex(2);
			ImGui::Text(Texture::TypeNames.find(i->second.type)->second.c_str());
		}
		ImGui::EndTable();
	}
	ImGui::NewLine();

	for (auto i = ResourceManager::materials.begin(); i != ResourceManager::materials.end(); i++)
	{
		for (auto j = i->second.textures.begin(); j != i->second.textures.end(); j++)
		{
			ImGui::Text(std::to_string((*j)->ID).c_str());
			ImGui::SameLine();
		}
		ImGui::NewLine();
	}
	ImGui::NewLine();

	for (auto i = ResourceManager::shaders.begin(); i != ResourceManager::shaders.end(); i++)
	{
		ImGui::Text(i->first.c_str());
		ImGui::SameLine();
		ImGui::Text(std::to_string(i->second.ID).c_str());
	}
}


void ResourceManager::UnloadAll()
{
	for (auto i = textures.begin(); i != textures.end(); i++)
	{
		i->second.DeleteTexture();
	}
	textures.clear();

	for (auto i = shaders.begin(); i != shaders.end(); i++)
	{
		i->second.DeleteProgram();
	}
	shaders.clear();
}
