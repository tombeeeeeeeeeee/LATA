#include "ResourceManager.h"

#include "Utilities.h"

#include "stb_image.h"
#include "imgui.h"
#include "imgui_stdlib.h"

#include <iostream>

std::unordered_map<unsigned long long, Texture, ResourceManager::hashFNV1A> ResourceManager::idTextureMap;
std::unordered_map<std::string, Texture*, ResourceManager::hashFNV1A> ResourceManager::pathTextureMap;
unsigned long long ResourceManager::guidCounter = 100;
std::unordered_map<std::string, Shader, ResourceManager::hashFNV1A> ResourceManager::shaders;
std::unordered_map<std::string, Material, ResourceManager::hashFNV1A> ResourceManager::materials;


const unsigned long long ResourceManager::hashFNV1A::offset = 14695981039346656037;
const unsigned long long ResourceManager::hashFNV1A::prime = 1099511628211;


Texture* ResourceManager::GetTexture(std::string path, Texture::Type type, int wrappingMode, bool flipOnLoad)
{
	auto texture = pathTextureMap.find(path);

	if (texture == pathTextureMap.end()) {
		Texture newTexture(path, type, wrappingMode, flipOnLoad);
		newTexture.GUID = GetNewGuid();
		// TODO: Call load texture function here instead
		texture = pathTextureMap.emplace(path, &newTexture).first;
		idTextureMap.emplace(newTexture.GUID, newTexture);
	}

	return texture->second;
}

Texture* ResourceManager::GetTexture(unsigned long long GUID)
{
		auto texture = idTextureMap.find(GUID);
	
		if (texture == idTextureMap.end()) {
			std::cout << "Error: Unable to find texture of GUID: " << GUID << "\n";
			return nullptr; // return 'default' or 'missing' texture instead
		}
	
		return &texture->second;
}

unsigned long long ResourceManager::LoadTexture(std::string path, Texture::Type type, int wrappingMode, bool flipOnLoad)
{
	Texture newTexture = Texture(path, type, wrappingMode, flipOnLoad);
	newTexture.GUID = GetNewGuid();
	idTextureMap.emplace(newTexture.GUID, newTexture);
	return newTexture.GUID;
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

// TODO: check if the prime and offset is supposed to be different for this one?
unsigned long long ResourceManager::hashFNV1A::operator()(unsigned long long key) const
{
	unsigned long long hash = offset;

	hash ^= key;
	hash *= prime;

	return hash;
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
		hash ^= (*t)->GLID;
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

		if (ImGui::BeginTable("Resource Textures", 5)) {
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Path");
			
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("Type");

			ImGui::TableSetColumnIndex(2);
			ImGui::Text("Flipped");

			ImGui::TableSetColumnIndex(3);
			ImGui::Text("ID");

			ImGui::TableSetColumnIndex(4);
			ImGui::Text("Reload");

			for (auto i = ResourceManager::idTextureMap.begin(); i != ResourceManager::idTextureMap.end(); i++)
			{
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::PushItemWidth(270);
				ImGui::InputText(("##" + PointerToString(&i->second.path)).c_str(), &i->second.path); // TODO:

				ImGui::TableSetColumnIndex(1);

				ImGui::PushItemWidth(90);
				if (ImGui::BeginCombo(("##" + PointerToString(&i->second)).c_str(), Texture::TypeNames.find(i->second.type)->second.c_str()))
				{
					for (int n = 0; n < (int)Texture::Type::count; n++)
					{
						const bool is_selected = ((int)i->second.type == n);
						if (ImGui::Selectable(Texture::TypeNames.find((Texture::Type)n)->second.c_str(), is_selected)) {
							i->second.type = (Texture::Type)n;
						}

						// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
						if (is_selected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}

				ImGui::TableSetColumnIndex(2);
				ImGui::Checkbox(("##" + PointerToString(&i->second.flipped)).c_str(), &i->second.flipped);

				ImGui::TableSetColumnIndex(3);
				ImGui::Text(std::to_string(i->second.GLID).c_str());

				ImGui::TableSetColumnIndex(4);
				if (ImGui::Button(("Reload##" + std::to_string(i->second.GLID)).c_str())) {
					i->second.Load();
				}
			}
			ImGui::EndTable();
		}
	}

	// TODO: Should be able to change the stuff of materials
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
		if (ImGui::BeginTable("Shader list", 4)) {
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Vertex");

			ImGui::TableSetColumnIndex(1);
			ImGui::Text("Fragment");

			ImGui::TableSetColumnIndex(2);
			ImGui::Text("ID");

			ImGui::TableSetColumnIndex(3);
			ImGui::Text("Reload");
		}
		for (auto i = ResourceManager::shaders.begin(); i != ResourceManager::shaders.end(); i++)
		{
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::PushItemWidth(180);
			ImGui::InputText(("##Vertex" + PointerToString(&i->second.vertexPath)).c_str(), &i->second.vertexPath);

			ImGui::TableSetColumnIndex(1);
			ImGui::PushItemWidth(180);
			ImGui::InputText(("##Fragment" + PointerToString(&i->second.fragmentPath)).c_str(), &i->second.fragmentPath);

			ImGui::TableSetColumnIndex(2);
			ImGui::Text(std::to_string(i->second.GLID).c_str());

			ImGui::TableSetColumnIndex(3);
			if (ImGui::Button(("Recompile##" + std::to_string(i->second.GLID)).c_str())) {
				i->second.Load();
			}
		}
		ImGui::EndTable();
	}
}


unsigned long long ResourceManager::GetNewGuid()
{
	return ++guidCounter;
}

void ResourceManager::UnloadAll()
{
	for (auto i = idTextureMap.begin(); i != idTextureMap.end(); i++)
	{
		i->second.DeleteTexture();
	}
	idTextureMap.clear();

	for (auto i = shaders.begin(); i != shaders.end(); i++)
	{
		i->second.DeleteProgram();
	}
	shaders.clear();
}
