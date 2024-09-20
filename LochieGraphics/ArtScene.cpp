#include "ArtScene.h"

#include "ResourceManager.h"
#include "SceneManager.h"
#include "Skybox.h"
#include "Camera.h"
#include "SceneObject.h"
#include "Paths.h"
#include "ShaderEnum.h"

#include "Utilities.h"

#include "EditorGUI.h"
#include "Serialisation.h"

#include "stb_image.h"
#include "stb_image_write.h"

#include <ios>
#include <fstream>
#include <ostream>
#include <iostream>
#include <filesystem>

ArtScene* ArtScene::artScene = nullptr;


std::string ArtScene::EnsureCorrectFileLocation(const std::string& path, const std::string& expected) const
{
	// Copy the file into the expected path
	// Return the path to the file in expected

	std::string newPath = expected + Utilities::FilenameFromPath(path, true);
	if (newPath == path) { return newPath; }
	// TODO: Look into a better way to have this writted, the try catch feels bad
	try
	{
		if (std::filesystem::equivalent(std::filesystem::path(newPath), std::filesystem::path(path)))
		{
			// Checks if dragging in from the expected location, if so no action is needed
			return newPath;
		}
	}
	catch (const std::filesystem::filesystem_error)
	{
	}	
	
	std::cout << "Copied file for locality, from, to:\n" << path << "\n" << newPath << '\n';
	// TODO: Consider using filesystem function to copy file
	std::ifstream original(path, std::ios::binary);
	std::ofstream copied(newPath, std::ios::binary);

	copied << original.rdbuf();	

	original.close();
	copied.close();

	return newPath;
}

void ArtScene::RefreshPBR()
{
	// TODO: Clean up old PBRs

	int width = 0;
	int height = 0;
	std::string name;

	bool foundImage = false;

	for (auto& i : importImages)
	{
		if (!i.second->loaded) { continue; }
		if (foundImage) {
			if (width != i.second->width || height != i.second->height || name != MaterialNameFromTexturePath(i.second->path)) {
				std::cout << "Mismatched PBR texture images " << i.second->path << " does not match with the other last given texture (in terms of either size or name)\n"
					<< "this can be ignored if in process of inputting a new batch of textures\n";
				return;
			}
		}
		width = i.second->width;
		height = i.second->height;
		foundImage = true;
		name = MaterialNameFromTexturePath(i.second->path);
	}


	if (width == 0 || height == 0) {
		return;
	}
	unsigned int size = (unsigned int)width * (unsigned int)height;
	unsigned char pbrC = 4;
	
	std::vector<unsigned char> data(size * pbrC);

	for (size_t i = 0; i < size; i++)
	{
		if (metallicImage.loaded) {
			data[i * pbrC + 0] = metallicImage.data[i];
		}
		else {
			data[i * pbrC + 0] = missingMetallicValue;
		}

		if (roughnessImage.loaded) {
			data[i * pbrC + 1] = roughnessImage.data[i];
		}
		else {
			data[i * pbrC + 1] = missingRoughnessValue;
		}

		if (aoImage.loaded) {
			data[i * pbrC + 2] = aoImage.data[i];
		}
		else {
			data[i * pbrC + 2] = missingAoValue;
		}
		data[i * pbrC + 3] = UCHAR_MAX;
	}

	//pbr = ResourceManager::LoadTexture(width, height, GL_SRGB_ALPHA, data.data(), GL_REPEAT, GL_UNSIGNED_BYTE, true);

	// TODO: Get name from base image
	std::string filename = Paths::importTextureLocation + texturePrefix + name + "_PBR.tga"; // 
	int result = stbi_write_tga(("./" + filename).c_str(), width, height, STBI_rgb_alpha, data.data());
	Texture* pbr = ResourceManager::LoadTexture(filename, Texture::Type::PBR);
	material->AddTextures({ pbr });

	//int tW, tH, tC;
	
	//unsigned char* test = stbi_load("./newPBR.tga", &tW, &tH, &tC, STBI_rgb_alpha);

	std::cout << "Wrote PBR image, with result of " << result << '\n';

	texturePreviewScale = 128.0f / width;
}

// TODO: These should only get generated at the end of an import, for example now if all three of the textures get dropped in this will get called thrice, instead of once at the end
void ArtScene::RefreshPBRComponents()
{
	// Refresh the PBR preview textures

	for (auto& i : importImages)
	{
		if (i.second->loaded) {
			(*importTextures.at(i.first))->setWidthHeight(i.second->width, i.second->height);
			
			//(*importTextures.at(i.first))->path = i.second->path;

			
			std::vector<unsigned char> data(4 * (*importTextures.at(i.first))->width * (*importTextures.at(i.first))->height);
			for (auto j = 0; j < i.second->width * i.second->height; j++)
			{
				data[j * 4 + 0] = i.second->data[j];
				data[j * 4 + 1] = i.second->data[j];
				data[j * 4 + 2] = i.second->data[j];
				data[j * 4 + 3] = i.second->data[j];
			}

			(*importTextures.at(i.first))->Load(data.data());

			//(*importTextures.at(i.first))->Load();
		}
		else {

			// TODO: Get a preview texture from the currently loaded PBR texture?
		}
	}
	RefreshPBR();
}

void ArtScene::ResetCamera()
{
	float distance = (model->max.y - model->min.y) / tanf(resetCamObjectViewSpace);
	camera->transform.setPosition({ distance + model->max.x, (model->min.y + model->max.y) / 2, (model->min.z + model->max.z) / 2});
	camera->transform.setEulerRotation({ 0.0f, 180.0f, 0.0f });
	camera->editorSpeed.move = glm::length(model->max - model->min) / 3;
	camera->artFocusDistance = distance + ((model->max.y - model->min.y) / 2);
}

void ArtScene::DragDropCallback(GLFWwindow* window, int pathCount, const char* paths[])
{
	ArtScene::artScene->ImportFromPaths(pathCount, paths);
}

void ArtScene::ImportFromPaths(int pathCount, const char* paths[])
{
	for (int i = 0; i < pathCount; i++)
	{
		std::string path = paths[i];
		std::string filename = path.substr(path.find_last_of("/\\") + 1);
		if (filename._Starts_with(texturePrefix))
		{
			ImportTexture(path, filename);
			continue;
		}
		else if (filename._Starts_with(meshPrefix)) {
			ImportMesh(path, filename);
			continue;
		}
		// Checking if the path is folder
		// TODO: Maybe do this properly with some filesystem thing instead
		auto lastPeriod = path.find_last_of(".");
		if (lastPeriod == std::string::npos) {
			lastPeriod = 0;
		}
		if (path.find_last_of("/\\") > lastPeriod) {
			ImportFolder(path);
			continue;
		}
		std::cout << "Unknown item dropped! Could not detect type based on prefix: " << path << "\n";
	}
}

void ArtScene::ImportTexture(std::string& path, std::string& filename)
{
	// Figure out texture type
	Texture::Type type = Texture::Type::count;

	for (auto& t : acceptableImportTypeNames) {
		if (filename.find("_" + t.first) != std::string::npos) {
			type = t.second;
			break;
		}
	}
	if (type == Texture::Type::count) {
		std::cout << "Unknown texture type! Couldn't find what " << filename << " is supposed to be!\n";
		return;
	}

	// Load texture
	Texture* newTexture;
	switch (type)
	{
	case Texture::Type::albedo: case Texture::Type::normal: case Texture::Type::emission:
		newTexture = ResourceManager::LoadTexture(EnsureCorrectFileLocation(path, Paths::importTextureLocation), type, GL_REPEAT, defaultFlip);
		material->AddTextures(std::vector<Texture*>{ newTexture });
		// Refresh texture preview size
		texturePreviewScale = std::min((loadTargetPreviewSize / std::max(newTexture->width, newTexture->height)), texturePreviewScale);
		break;

	case Texture::Type::roughness:
		roughnessImage.Load(path);
		RefreshPBRComponents();
		break;
	case Texture::Type::metallic:
		metallicImage.Load(path);
		RefreshPBRComponents();
		break;
	case Texture::Type::ao:
		aoImage.Load(path);
		RefreshPBRComponents();
		break;
	// TODO:
	case Texture::Type::PBR: //
		break;
	case Texture::Type::paint: default:
		break;
	}
}

void ArtScene::ImportMesh(std::string& path, std::string& filename)
{
	// TODO: delete old model
	//model->meshes.clear();

	model = ResourceManager::LoadModel(EnsureCorrectFileLocation(path, Paths::importModelLocation));

	sceneObject->renderer()->modelGUID = model->GUID;
	sceneObject->renderer()->Refresh();

	ResetCamera();
}

void ArtScene::ImportFolder(std::string& path)
{
	stringPaths.clear();
	newPaths.clear();
	for (const auto& entry : std::filesystem::directory_iterator(path)) {
		stringPaths.push_back(entry.path().string());
		newPaths.push_back(stringPaths.back().c_str());
	}
	ImportFromPaths((int)newPaths.size(), newPaths.data());
}

ArtScene::ArtScene() :
	sceneObject(new SceneObject(this, "Editing Scene Object"))
{
	artScene = this;
}

void ArtScene::Start()
{

	std::string skyboxPaths[6] = {
		"images/black.png",
		"images/black.png",
		"images/black.png",
		"images/black.png",
		"images/black.png",
		"images/black.png",
	};

	auto temp = Texture::LoadCubeMap(&skyboxPaths[0]);
	skybox = new Skybox(shaders[skyBoxShader], temp);

	model = ResourceManager::LoadModel();
	material = ResourceManager::LoadMaterial("New Material", shaders[super]);
	unsigned long long GUID = material->GUID;
	*material = *ResourceManager::defaultMaterial;
	material->GUID = GUID;
	material->name = "New Material";

	ModelRenderer* modelRenderer = new ModelRenderer(model, material);
	sceneObject->setRenderer(modelRenderer);

	lights.insert(lights.end(), {
		&directionalLight,
		&spotlight,
		&pointLights[0],
		&pointLights[1],
		&pointLights[2],
		&pointLights[3],
		});

	glfwSetDropCallback(SceneManager::window, DragDropCallback);

	// TODO: These should not be one channel, and instead create the image so that it is white ( not red)
	metallicPreview = ResourceManager::LoadTexture(1024, 1024);
	roughnessPreview = ResourceManager::LoadTexture(1024, 1024);
	aoPreview = ResourceManager::LoadTexture(1024, 1024);

	importTextures["metallic"] = &metallicPreview;
	importTextures["roughness"] = &roughnessPreview;
	importTextures["ao"] = &aoPreview;

	importImages["metallic"] = &metallicImage;
	importImages["roughness"] = &roughnessImage;
	importImages["ao"] = &aoImage;

	for (auto& i: importTextures)
	{
		(*i.second)->format = GL_SRGB_ALPHA;
	}

	for (auto& i : importImages)
	{
		i.second->components = 1;
		// TODO: Ensure the correct flip-ness
		i.second->flipped = false;
	}

	camera->nearPlane = 1.0f;
	camera->farPlane = 3000.0f;
	camera->state = Camera::State::artEditorMode;
	camera->artState = Camera::ArtState::none;

	gui.sceneObjectSelected = sceneObject;
	gui.showSceneObject = true;
}

void ArtScene::Update(float delta)
{
}

void ArtScene::Draw()
{
	renderSystem.Update(
		renderers,
		transforms,
		renderers,
		animators,
		camera
	);
}

void ArtScene::GUI()
{
	if (ImGui::Begin("Art Stuff", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {

		if (ResourceManager::MaterialSelector("Editing Material", &material, shaders[super], false)) {
			// TODO: Refresh the preview materials
		}

		if (ImGui::CollapsingHeader("Current Material")) {
			ImGui::SliderFloat("Preview Scale", &texturePreviewScale, 0.01f, 1.0f, "% .3f", ImGuiSliderFlags_Logarithmic);

			ImGui::BeginChild("Textures", ImVec2(0, 0), ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_AlwaysHorizontalScrollbar);
			for (auto& i : importTextures)
			{
				ImGui::BeginGroup();
				ImGui::Text(i.first.c_str());
				if ((*i.second) != nullptr) {
					ImGui::Image((ImTextureID)(unsigned long long)(*i.second)->GLID, { texturePreviewScale * (float)(*i.second)->width, texturePreviewScale * (float)(*i.second)->height });
				}
				ImGui::EndGroup();
				ImGui::SameLine();
			}
			ImGui::BeginGroup();

			ImGui::Text("PBR");
			Texture* pbr = (material->getFirstTextureOfType(Texture::Type::PBR));
			if (pbr != nullptr) {
				ImGui::Image((ImTextureID)(unsigned long long)pbr->GLID, { texturePreviewScale * (float)pbr->width, texturePreviewScale * (float)pbr->height });
			}

			ImGui::EndGroup();
			ImGui::NewLine();
			ImGui::EndChild();
		}

		if (ImGui::SliderFloat("Target Object View Space", &resetCamObjectViewSpace, 0.15f, PI/2.0f, "", ImGuiSliderFlags_Logarithmic)) {
			ResetCamera();
		}
	}
	ImGui::End();

	if (!gui.showSceneObject) {
		return;
	}
	if (ImGui::Begin("Scene Object Menu")) {
		if (ImGui::Button("SAVE AS ASSET")) {
			openSave = true;
		}
	}
	ImGui::End();

	SaveModal();
}

void ArtScene::SaveModal()
{
	ModelRenderer* renderer = sceneObject->renderer();

	if (openSave) {
		ImGui::OpenPopup("Save Assets");
		openSave = false;

		materialsToSave.clear();
		texturesToSave.clear();

		for (size_t i = 0; i < renderer->materials.size(); i++)
		{
			for (auto& m : materialsToSave)
			{
				if (m.first == renderer->materials[i]) {
					continue;
				}
			}
			if (renderer->materials[i] == nullptr) { continue; }
			materialsToSave.push_back({ renderer->materials[i], true });
			for (auto& t : renderer->materials[i]->texturePointers)
			{
				if (std::find(texturesToSave.begin(), texturesToSave.end(), std::pair<std::pair<std::string, Texture*>, bool>{ {t.first, t.second}, true }) == texturesToSave.end()) {
					if (t.second != nullptr) { texturesToSave.push_back({ {t.first, t.second}, true }); }
				}
			}
		}
	}
	if (!ImGui::BeginPopupModal("Save Assets", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		return;
	}

	// Show everything that can be saved, with the name/filepath it'll have

	ImGui::BeginTable("Saving List", 3);

	ImGui::TableNextRow();

	ImGui::TableSetColumnIndex(0);
	ImGui::PushItemWidth(200.0f);
	ImGui::Text("Name");
	ImGui::TableSetColumnIndex(1);
	ImGui::Text("Type");
	ImGui::TableSetColumnIndex(2);
	ImGui::Text("Save");

	ImGui::TableNextRow();

	ImGui::TableSetColumnIndex(0);
	ImGui::InputText(("##name" + Utilities::PointerToString(&sceneObject->name)).c_str(), &sceneObject->name, ImGuiInputTextFlags_AutoSelectAll);
	ImGui::TableSetColumnIndex(1);
	ImGui::Text("Renderer");

	ImGui::TableSetColumnIndex(2);
	ImGui::Checkbox(("##saving" + Utilities::PointerToString(renderer)).c_str(), &saveRenderer);

	ImGui::TableNextRow();

	ImGui::TableSetColumnIndex(0);
	ImGui::Text(model->path.c_str());
	ImGui::TableSetColumnIndex(1);
	ImGui::Text("Model");

	ImGui::TableSetColumnIndex(2);
	ImGui::Checkbox(("##saving" + Utilities::PointerToString(model)).c_str(), &saveModel);


	ImGui::TableNextRow();

	// For each Material
	// TODO: Iterator loop
	for (size_t i = 0; i < materialsToSave.size(); i++)
	{
		Material* material = materialsToSave[i].first;
		ImGui::TableSetColumnIndex(0);

		ImGui::InputText(("##name" + Utilities::PointerToString(&material->name)).c_str(), &material->name, ImGuiInputTextFlags_AutoSelectAll);

		ImGui::TableSetColumnIndex(1);
		ImGui::Text("Material");

		ImGui::TableSetColumnIndex(2);
		ImGui::Checkbox(("##saving" + Utilities::PointerToString(material)).c_str(), &materialsToSave[i].second);

		ImGui::TableNextRow();
	}

	// For each Texture
	for (auto i = texturesToSave.begin(); i != texturesToSave.end(); i++)
	{
		ImGui::TableSetColumnIndex(0);
		if (i->first.second) {
			ImGui::Text(i->first.second->path.c_str());
		}

		ImGui::TableSetColumnIndex(1);
		ImGui::Text("Texture");

		ImGui::TableSetColumnIndex(2);
		bool checked = i->second;
		if (ImGui::Checkbox(("##saving" + Utilities::PointerToString(&i->first.first)).c_str(), &checked)) {
			i->second = checked;
		}

		ImGui::TableNextRow();
	}

	ImGui::EndTable();

	if (ImGui::Button("Cancel##saving")) {
		ImGui::CloseCurrentPopup();
	}
	ImGui::SameLine();
	if (ImGui::Button("Save##ArtAssets")) {
		ImGui::CloseCurrentPopup();
		SaveArtAsset();
	}

	ImGui::EndPopup();
}

std::string ArtScene::MaterialNameFromTexturePath(std::string& path)
{
	std::string filename = Utilities::FilenameFromPath(path);
	unsigned long long start = filename.find_first_of('_') + 1;
	unsigned long long end = filename.find_last_of('_');
	return filename.substr(start, end - start);
}

ArtScene::~ArtScene()
{

}

void ArtScene::SaveArtAsset()
{
	if (saveRenderer) {
		std::ofstream file(Paths::rendererSaveLocation + sceneObject->name + Paths::rendererExtension);
		// The renderer itself does not need to save its GUID
		file << sceneObject->renderer()->Serialise(0);
		file.close();
	}

	for (auto& i : materialsToSave)
	{
		if (!i.second) { continue; }
		std::ofstream file(Paths::materialSaveLocation + i.first->name + Paths::materialExtension);
		file << i.first->Serialise();
		file.close();
	}

	for (auto& i : texturesToSave)
	{
		if (!i.second) { continue; }
		std::ofstream file(Paths::textureSaveLocation + MaterialNameFromTexturePath(i.first.second->path) + '_' + Texture::TypeNames.at(i.first.second->type) + Paths::textureExtension);
		file << i.first.second->Serialise();
		file.close();
	}

	if (saveModel) {
		std::ofstream file(Paths::modelSaveLocation + Utilities::FilenameFromPath(model->path, false) + Paths::modelExtension);
		file << model->Serialise();
		file.close();
	}
}

void ArtScene::Save()
{
	openSave = true;
}

void ArtScene::Load()
{
	// Load the whole art asset
}
