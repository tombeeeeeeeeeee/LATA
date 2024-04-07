#include "ResourceManager.h"

#include "Utilities.h"

#include "stb_image.h"
#include "imgui.h"

#include <iostream>

std::unordered_map<std::string, Texture, ResourceManager::hashFNV1A> ResourceManager::textures;
std::unordered_map<std::string, Shader, ResourceManager::hashFNV1A> ResourceManager::shaders;
std::unordered_map<std::string, Material, ResourceManager::hashFNV1A> ResourceManager::materials;


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
		Shader newShader(vertexPath, fragmentPath);

		shader = shaders.emplace(path, newShader).first;
	}

	return &shader->second;
}

Material* ResourceManager::GetMaterial(std::string name, Shader* shader)
{
	auto material = materials.find(name);

	if (material == materials.end()) {
		Material newMaterial(name, shader);

		material = materials.emplace(name, newMaterial).first;
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
	if (ImGui::CollapsingHeader("Textures")) {

		if (ImGui::BeginTable("Resource Textures", 3)) {
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Path");
			
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("Type");

			ImGui::TableSetColumnIndex(2);
			ImGui::Text("ID");

			for (auto i = ResourceManager::textures.begin(); i != ResourceManager::textures.end(); i++)
			{
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Text(i->first.c_str());

				ImGui::TableSetColumnIndex(1);
				ImGui::Text(Texture::TypeNames.find(i->second.type)->second.c_str());

				ImGui::TableSetColumnIndex(2);
				ImGui::Text(std::to_string(i->second.ID).c_str());
			}
			ImGui::EndTable();
		}
	}

	if (ImGui::CollapsingHeader("Materials")) {

		for (auto i = ResourceManager::materials.begin(); i != ResourceManager::materials.end(); i++)
		{
			i->second.GUI();
			ImGui::NewLine();




			//ImGui::Text(i->first.c_str());
			//for (auto j = i->second.textures.begin(); j != i->second.textures.end(); j++)
			//{
			//	if (j->second == nullptr) { continue; }
			//	ImGui::Text(std::to_string((j->second)->ID).c_str());
			//	ImGui::SameLine();
			//}
			//ImGui::NewLine();
		}
	}
	
	if (ImGui::CollapsingHeader("Shaders")) {
		for (auto i = ResourceManager::shaders.begin(); i != ResourceManager::shaders.end(); i++)
		{
			ImGui::Text(i->first.c_str());
			ImGui::SameLine();
			ImGui::Text(std::to_string(i->second.ID).c_str());
		}
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
