#include "ResourceManager.h"

#include "Utilities.h"

#include "stb_image.h"
#include "imgui.h"
#include "imgui_stdlib.h"

#include <iostream>

using Utilities::PointerToString;

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

Shader* ResourceManager::LoadShader(std::string sharedName, int flags)
{
	LoadResource(Shader, shaders, "shaders/" + sharedName + ".vert", "shaders/" + sharedName + ".frag", flags);
}

Shader* ResourceManager::LoadShaderDefaultVert(std::string fragmentName, int flags)
{
	LoadResource(Shader, shaders, "shaders/default.vert", "shaders/" + fragmentName + ".frag", flags);
}

Texture* ResourceManager::LoadTexture(std::string path, Texture::Type type, int wrappingMode, bool flipOnLoad)
{
	LoadResource(Texture, textures, path, type, wrappingMode, flipOnLoad);
}

Texture* ResourceManager::LoadTexture(unsigned int width, unsigned int height, GLenum format, unsigned char* data, GLint wrappingMode, GLenum dataType, bool mipMaps, GLint minFilter, GLint magFilter)
{
	LoadResource(Texture, textures, width, height, format, data, wrappingMode, dataType, mipMaps, minFilter, magFilter);
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
// TODO: This code needs to be moved to respective classes or something
void ResourceManager::GUI()
{
	if (ImGui::CollapsingHeader("Textures")) {

		
		std::vector<Texture*> tempTextures = {};
		for (auto i = textures.begin(); i != textures.end(); i++)
		{
			tempTextures.push_back(&i->second);
		}
		Texture::EditorGUI(tempTextures);
	}

	// TODO: Should be able to change the stuff of materials
	if (ImGui::CollapsingHeader("Materials")) {

		for (auto i = materials.begin(); i != materials.end(); i++)
		{
			i->second.GUI();
			ImGui::NewLine();
		}
	}
	
	// TODO: GUI for shader flags, there is a built in imgui thing
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
		for (auto i = shaders.begin(); i != shaders.end(); i++)
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
				i->second.Load(); //TODO: When a shader reloads the materials need to also reload
				BindFlaggedVariables();
			}
		}

		ImGui::EndTable();

		if (ImGui::Button("Create new Shader")) {
			LoadShader("", "", Shader::Flags::None); //TODO: Use some template shader
		}

	}
}


unsigned long long ResourceManager::GetNewGuid()
{
	return ++guidCounter;
}

void ResourceManager::BindFlaggedVariables()
{
	for (auto i = shaders.begin(); i != shaders.end(); i++)
	{
		int flag = (*i).second.getFlag();
		(*i).second.Use();

		if (flag & Shader::Flags::Spec)
		{
			(*i).second.setInt("irradianceMap", 7);
			(*i).second.setInt("prefilterMap", 8);
			(*i).second.setInt("brdfLUT", 9);
		}
		if (flag & Shader::Flags::Painted)
		{
			(*i).second.setInt("brushStrokes", 10);
		}
	}
}

void ResourceManager::BindFlaggedVariables(Shader* shader)
{
	int flag = shader->getFlag();
	shader->Use();

	if (flag & Shader::Flags::Spec)
	{
		shader->setInt("irradianceMap", 7);
		shader->setInt("prefilterMap", 8);
		shader->setInt("brdfLUT", 9);
	}
	if (flag & Shader::Flags::Painted)
	{
		shader->setInt("brushStrokes", 10);
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

	materials.clear();
}
