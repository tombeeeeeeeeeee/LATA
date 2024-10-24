#include "ResourceManager.h"

#include "Paths.h"
#include "ShaderEnum.h"
#include "Vertex.h"
#include "UserPreferences.h"

#include "Utilities.h"

#include "ExtraEditorGUI.h"
#include "Serialisation.h"

#include <fstream>
#include <filesystem>
#include <iostream>
#include <chrono>

using Utilities::PointerToString;

std::unordered_map<unsigned long long, Texture, hashFNV1A> ResourceManager::textures;
std::unordered_map<unsigned long long, Shader, hashFNV1A> ResourceManager::shaders;
std::unordered_map<unsigned long long, Material, hashFNV1A> ResourceManager::materials;
std::unordered_map<unsigned long long, Model, hashFNV1A> ResourceManager::models;
std::unordered_map<unsigned long long, Mesh, hashFNV1A> ResourceManager::meshes;

Texture* ResourceManager::defaultTexture = nullptr;
Shader* ResourceManager::defaultShader = nullptr;
Material* ResourceManager::defaultMaterial = nullptr;
Model* ResourceManager::defaultModel = nullptr;
Mesh* ResourceManager::defaultMesh = nullptr;

unsigned long long ResourceManager::guidCounter = 100;
std::random_device ResourceManager::guidRandomiser = {};

#define LoadResource(type, collection, ...)                             \
type newResource = type(__VA_ARGS__ );                                  \
newResource.GUID = GetNewGuid();                                        \
return &collection.emplace(newResource.GUID, newResource).first->second \

Shader* ResourceManager::LoadShader(std::string vertexPath, std::string fragmentPath, int flags)
{
	LoadResource(Shader, shaders, vertexPath, fragmentPath, flags);
}

// TODO: These paths and extensions should be in the Paths.h
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

Shader* ResourceManager::LoadShader(toml::v3::table toml)
{
	return LoadShader(&toml);
}

#define LoadResourceAsset(type, container)                                                \
	std::ifstream file(path);                                                             \
	if (!file) {                                                                          \
		std::cout << "Failed to load ##type asset, attempted at path: " << path << '\n';  \
		return nullptr;                                                                   \
	}                                                                                     \
	toml::table data = toml::parse(file);                                                 \
	unsigned long long loadingGUID = Serialisation::LoadAsUnsignedLongLong(data["guid"]); \
	auto search = container.find(loadingGUID);                                            \
	if (search != container.end()) {                                                      \
		return &search->second;                                                           \
	}                                                                                     \
	file.close();                                                                         \
	return &container.emplace(loadingGUID, data).first->second;

Shader* ResourceManager::LoadShaderAsset(std::string path)
{
	LoadResourceAsset(Shader, shaders);
}

Model* ResourceManager::LoadModel(std::string path)
{
	LoadResource(Model, models, path);
}

Model* ResourceManager::LoadModelAsset(std::string path)
{
	LoadResourceAsset(Model, models);
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

Texture* ResourceManager::LoadTextureAsset(std::string path)
{
	LoadResourceAsset(Texture, textures);
}

Material* ResourceManager::LoadMaterial(std::string name, Shader* shader)
{
	LoadResource(Material, materials, name, shader);
}

Material* ResourceManager::LoadDefaultMaterial()
{
	LoadResource(Material, materials, "New Material", defaultShader);
}

Material* ResourceManager::LoadMaterialAsset(std::string path)
{
	LoadResourceAsset(Material, materials);
}

std::unordered_map<unsigned long long, Material, hashFNV1A>& ResourceManager::getMaterials()
{
	return materials;
}

#define ResourceSelector(Type, selector, label, createFunction)    \
	std::string tag = Utilities::PointerToString(selector); \
	std::vector<Type*> pointers;                            \
	for (auto& i : ##selector##s)                           \
	{                                                       \
		pointers.push_back(&i.second);                      \
	}                                                       \
	return ExtraEditorGUI::InputSearchBox(pointers.begin(), pointers.end(), selector, label, tag, showNull, createFunction)


bool ResourceManager::TextureSelector(std::string label, Texture** texture, bool showNull)
{
	ResourceSelector(Texture, texture, label, (Texture * (*)())nullptr);
}

bool ResourceManager::ShaderSelector(std::string label, Shader** shader, bool showNull)
{
	ResourceSelector(Shader, shader, label, (Shader*(*)())nullptr);
}

bool ResourceManager::MaterialSelector(std::string label, Material** material, Shader* newMaterialShader, bool showNull)
{
	ResourceSelector(Material, material, label, LoadDefaultMaterial);
}

bool ResourceManager::ModelSelector(std::string label, Model** model, bool showNull)
{
	if (UserPreferences::modelSelectMode == UserPreferences::ModelSelectMode::loaded) {
		ResourceSelector(Model, model, label, (Model * (*)())nullptr);
	}
	else if (UserPreferences::modelSelectMode == UserPreferences::ModelSelectMode::assets) {
		std::string tag = Utilities::PointerToString(model);
		std::vector<std::string> paths;
		std::vector<std::string*> pointers;
		std::string* modelPath = nullptr;
		for (auto& i : std::filesystem::directory_iterator(Paths::modelSaveLocation)) {
			paths.push_back(Utilities::FilenameFromPath(i.path().string(), false));
		}
		for (size_t i = 0; i < paths.size(); i++)
		{
			pointers.push_back(&paths.at(i));
			if (*model) {
				if (Utilities::FilenameFromPath((*model)->path, false) == paths.at(i)) {
					modelPath = pointers.back();
				}
			}
		}
		if (ExtraEditorGUI::InputSearchBox(pointers.begin(), pointers.end(), &modelPath, "Model", tag, false)) {
			*model = LoadModelAsset(Paths::modelSaveLocation + *modelPath + Paths::modelExtension);
			return true;
		}
		return false;
	}
	std::cout << "Unknown Model Selector Type\n";
	return false;
}

#define GetResource(type, collection)                    \
type* ResourceManager::Get##type(unsigned long long GUID)\
{                                                        \
	auto search = collection.find(GUID);                 \
	if (search == collection.end()){                     \
		return default##type;                            \
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
			LoadMaterial("", defaultShader);
		}

	}

	// TODO: GUI for shader flags, there is a built in imgui thing
	if (ImGui::CollapsingHeader("Shaders")) {
		if (ImGui::BeginTable("Shader list", 8)) {
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
		
			ImGui::TableSetColumnIndex(7);
			ImGui::Text("Save");
		}
		for (auto i = shaders.begin(); i != shaders.end(); i++)
		{
			std::string tag = Utilities::PointerToString(&i->second);
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
			if (ImGui::Button(("R##" + tag).c_str())) {
				i->second.Load(); //TODO: When a shader reloads the materials need to also reload
				BindFlaggedVariables();
			}

			ImGui::TableSetColumnIndex(7);
			if (ImGui::Button(("S##" + tag).c_str())) {
				i->second.SaveAsAsset();
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
	// TODO: Generate this properly
	using namespace std::chrono;
	
	auto time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

	std::uniform_int_distribution<uint32_t> random;

	return (time << 32) + random(guidRandomiser);
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
		shader->setInt("ssao", 11);
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
