#include "ResourceManager.h"

#include "Utilities.h"

#include "stb_image.h"
#include "imgui.h"
#include "imgui_stdlib.h"

#include <iostream>

std::unordered_map<unsigned long long, Texture, ResourceManager::hashFNV1A> ResourceManager::textures;
std::unordered_map<unsigned long long, Shader, ResourceManager::hashFNV1A> ResourceManager::shaders;
std::unordered_map<unsigned long long, Material, ResourceManager::hashFNV1A> ResourceManager::materials;
unsigned long long ResourceManager::guidCounter = 100;

const unsigned long long ResourceManager::hashFNV1A::offset = 14695981039346656037;
const unsigned long long ResourceManager::hashFNV1A::prime = 1099511628211;

#define LoadResource(type, collection, ...)                            \
type newResource(__VA_ARGS__ );                                        \
newResource.GUID = GetNewGuid();                                       \
return &collection.emplace(newResource.GUID, newResource).first->second\

Shader* ResourceManager::LoadShader(std::string vertexPath, std::string fragmentPath, int flags)
{
	LoadResource(Shader, shaders, vertexPath, fragmentPath, flags);
}

Texture* ResourceManager::LoadTexture(std::string path, Texture::Type type, int wrappingMode, bool flipOnLoad)
{
	LoadResource(Texture, textures, path, type, wrappingMode, flipOnLoad);
}

Material* ResourceManager::LoadMaterial(std::string name, Shader* shader)
{
	LoadResource(Material, materials, name, shader);
}

#define GetResource(type, collection)                    \
type* ResourceManager::Get##type(unsigned long long GUID)\
{                                                        \
	auto search = collection.find(GUID);                 \
	if (search == collection.end()){                     \
		return nullptr;                                  \
	}                                                    \
	return &search->second;                              \
}

GetResource(Shader, shaders)
GetResource(Texture, textures)
GetResource(Material, materials)

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

ResourceManager::~ResourceManager()
{
	UnloadAll();
}

void ResourceManager::GUI()
{
	if (ImGui::CollapsingHeader("Textures")) {

		if (ImGui::BeginTable("Resource Textures", 6)) {
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Path");
			
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("Type");

			ImGui::TableSetColumnIndex(2);
			ImGui::Text("Flip");

			ImGui::TableSetColumnIndex(3);
			ImGui::Text("GUID");

			ImGui::TableSetColumnIndex(4);
			ImGui::Text("glID");

			ImGui::TableSetColumnIndex(5);
			ImGui::Text("Reload");

			for (auto i = ResourceManager::textures.begin(); i != ResourceManager::textures.end(); i++)
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
				ImGui::Text(std::to_string(i->second.GUID).c_str());

				ImGui::TableSetColumnIndex(4);
				ImGui::Text(std::to_string(i->second.GLID).c_str());

				ImGui::TableSetColumnIndex(5);
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
		}
	}
	
	// TODO: GUI for shader flags
	if (ImGui::CollapsingHeader("Shaders")) {
		if (ImGui::BeginTable("Shader list", 7)) {
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Vertex");

			ImGui::TableSetColumnIndex(1);
			ImGui::Text("Fragment");

			ImGui::TableSetColumnIndex(2);
			ImGui::Text("Lit");

			ImGui::TableSetColumnIndex(3);
			ImGui::Text("VP matrix");

			ImGui::TableSetColumnIndex(4);
			ImGui::Text("GUID");

			ImGui::TableSetColumnIndex(5);
			ImGui::Text("glID");

			ImGui::TableSetColumnIndex(6);
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
			bool lit = i->second.getFlag() & Shader::Flags::Lit;
			if (ImGui::Checkbox(("##Lit" + PointerToString(&i->second)).c_str(), &lit)) {
				i->second.setFlag(Shader::Flags::Lit, lit);
			}

			ImGui::TableSetColumnIndex(3);
			bool vp = i->second.getFlag() & Shader::Flags::VPmatrix;
			if (ImGui::Checkbox(("##vp" + PointerToString(&i->second)).c_str(), &vp)) {
				i->second.setFlag(Shader::Flags::VPmatrix, vp);
			}

			ImGui::TableSetColumnIndex(4);
			ImGui::Text(std::to_string(i->second.GUID).c_str());

			ImGui::TableSetColumnIndex(5);
			ImGui::Text(std::to_string(i->second.GLID).c_str());

			ImGui::TableSetColumnIndex(6);
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

	materials.clear();
}
