#include "ResourceManager.h"

#include "SceneManager.h"

#include "Utilities.h"

#include "stb_image.h"
#include "imgui.h"
#include "imgui_stdlib.h"

#include <iostream>

using Utilities::PointerToString;

std::unordered_map<unsigned long long, Texture, ResourceManager::hashFNV1A> ResourceManager::textures;
std::unordered_map<unsigned long long, Shader, ResourceManager::hashFNV1A> ResourceManager::shaders;
std::unordered_map<unsigned long long, Material, ResourceManager::hashFNV1A> ResourceManager::materials;
std::unordered_map<unsigned long long, Model, ResourceManager::hashFNV1A> ResourceManager::models;
std::unordered_map<unsigned long long, Mesh, ResourceManager::hashFNV1A> ResourceManager::meshes;
unsigned long long ResourceManager::guidCounter = 100;

const unsigned long long ResourceManager::hashFNV1A::offset = 14695981039346656037;
const unsigned long long ResourceManager::hashFNV1A::prime = 1099511628211;


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


#define LoadResource(type, collection, ...)                            \
type newResource = type(__VA_ARGS__ );                                        \
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


Shader* ResourceManager::LoadShader(toml::v3::table* toml)
{
	Shader newShader = Shader(toml);
	if (shaders.find(newShader.GUID) != shaders.end()) {
		shaders.erase(newShader.GUID);
	}
	return &shaders.emplace(newShader.GUID, newShader).first->second;
}

Model* ResourceManager::LoadModel(std::string path)
{
	LoadResource(Model, models, path);
}

// TODO: Clean
Model* ResourceManager::LoadModel()
{
	Model newResource = Model();
	newResource.GUID = GetNewGuid();
	return &models.emplace(newResource.GUID, newResource).first->second;
	//Model newResource;
	//newResource.GUID = GetNewGuid();
	//return &models.emplace(newResource.GUID, newResource).first->second;
}

Mesh* ResourceManager::LoadMesh(std::vector<Vertex> vertices, std::vector<GLuint> indices)
{
	LoadResource(Mesh, meshes, vertices, indices);
}

Mesh* ResourceManager::LoadMesh(unsigned int vertexCount, const Vertex* vertices, unsigned int indexCount, GLuint* indices)
{
	LoadResource(Mesh, meshes, vertexCount, vertices, indexCount, indices);
}

Mesh* ResourceManager::LoadMesh(Mesh::presets preset)
{
	LoadResource(Mesh, meshes, preset);
}

Mesh* ResourceManager::LoadMesh()
{
	LoadResource(Mesh, meshes);
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

const std::unordered_map<unsigned long long, Material, ResourceManager::hashFNV1A>& ResourceManager::getMaterials()
{
	return materials;
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
GetResource(Model, models)
GetResource(Mesh, meshes)

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

		if (ImGui::Button("Create new Material")) {
			LoadMaterial("", SceneManager::scene->shaders[ShaderIndex::super]);
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
			if (ImGui::Button(("R##" + std::to_string(i->second.GLID)).c_str())) {
				i->second.Load(); //TODO: When a shader reloads the materials need to also reload
				BindFlaggedVariables();
			}
		}

		ImGui::EndTable();

		if (ImGui::Button("Create new Shader")) {
			LoadShader("", "", Shader::Flags::None); //TODO: Use some template shader
		}

	}

	if (ImGui::CollapsingHeader("Models")) {

		for (auto& model : models)
		{
			// TODO: model gui
			//ImGui::Text(std::to_string(model.second.GUID).c_str());
			//model.second.
			ImGui::Indent();

			model.second.GUI();

			ImGui::Unindent();

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
		BindFlaggedVariables(&(*i).second);
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

void ResourceManager::UnloadShaders()
{
	for (auto i = shaders.begin(); i != shaders.end(); i++)
	{
		i->second.DeleteProgram();
	}
	shaders.clear();
}

void ResourceManager::UnloadAll()
{
	for (auto i = textures.begin(); i != textures.end(); i++)
	{
		i->second.DeleteTexture();
	}
	textures.clear();

	UnloadShaders();

	materials.clear();
}

void ResourceManager::RefreshAllMaterials()
{
	for (auto& i : materials)
	{
		i.second.Refresh();
	}
}
