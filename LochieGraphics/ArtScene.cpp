#include "ArtScene.h"

#include "ResourceManager.h"
#include "SceneManager.h"

#include "stb_image.h"
#include "stb_image_write.h"

#include <filesystem>

ArtScene* ArtScene::artScene = nullptr;

void ArtScene::RefreshPBR()
{
	if (pbr != nullptr) {
		pbr->DeleteTexture();
	}

	
	int width = 0;
	int height = 0;

	int mW, mH, mC;
	unsigned char* metallicData;
	if (metallic) {
		metallicData = stbi_load(metallic->path.c_str(), &mW, &mH, &mC, STBI_grey);
		width = mW;
		height = mH;
	}
	else {
		metallicData = {};
	}

	int rW, rH, rC;
	unsigned char* roughnessData;
	if (roughness) {
		roughnessData = stbi_load(roughness->path.c_str(), &rW, &rH, &rC, STBI_grey);
		width = rW;
		height = rH;
	}
	else {
		roughnessData = {};
	}

	int aW, aH, aC;
	unsigned char* ambientData;
	if (ao) {
		ambientData = stbi_load(ao->path.c_str(), &aW, &aH, &aC, STBI_grey);
		width = aW;
		height = aH;
	}
	else {
		ambientData = {};
	}

	if (width == 0 || height == 0) {
		return;
	}
	size_t size = (size_t)width * (size_t)height;
	unsigned char pbrC = 4;
	
	std::vector<unsigned char> data(size * pbrC);

	//if (metallic) {
	//	for (size_t i = 0; i < size; i++)
	//	{
	//		data[i * pbrC + 0] = metallicData[i];

	//	}
	//}
	//else {
	//	for (size_t i = 0; i < size; i++)
	//	{
	//		data[i * pbrC + 0] = missingMetallicValue;
	//	}
	//}

	for (size_t i = 0; i < size; i++)
	{
		int col = i % width;
		int row = i / height;

		if (metallic) {
			data[i * pbrC + 0] = metallicData[i];
		}
		else {
			data[i * pbrC + 0] = missingMetallicValue;
		}

		if (roughness) {
			data[i * pbrC + 1] = roughnessData[i];
		}
		else {
			data[i * pbrC + 1] = missingRoughnessValue;
		}

		if (ao) {
			data[i * pbrC + 2] = ambientData[i];
		}
		else {
			data[i * pbrC + 2] = missingAoValue;
		}
		data[i * pbrC + 3] = UCHAR_MAX;
	}


	pbr = ResourceManager::LoadTexture(width, height, GL_SRGB_ALPHA, data.data(), GL_REPEAT, GL_UNSIGNED_BYTE, true);
	pbr->type = Texture::Type::PBR;
	material->AddTextures({ pbr });

	// TODO: Get name from base image
	int result = stbi_write_tga("./newPBR.tga", width, height, STBI_rgb_alpha, data.data());

	int tW, tH, tC;
	unsigned char* test = stbi_load("./newPBR.tga", &tW, &tH, &tC, STBI_rgb_alpha);

	std::cout << "Wrote PBR image, with result of " << result << '\n';
}

void ArtScene::ResetCamera()
{
	float distance = (model->max.y - model->min.y) / tanf(resetCamObjectViewSpace);
	camera->transform.setPosition({ distance + model->max.x, (model->min.y + model->max.y) / 2, (model->min.z + model->max.z) / 2});
	camera->transform.setEulerRotation({ 0.0f, 180.0f, 0.0f });
	camera->movementSpeed = glm::length(model->max - model->min);
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

//TODO
// When a pbr component texture gets imported, reload the pbr texture
// And load a non flipped version of the texture for the preview slot
// If a pbr texture gets imported, then the other textures can be recreated from it

// Switch the editing material

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
	Texture* newTexture = ResourceManager::LoadTexture(path, type, GL_REPEAT, defaultFlip);

	texturePreviewScale = std::min((loadTargetPreviewSize / std::max(newTexture->width, newTexture->height)), texturePreviewScale);

	switch (type)
	{
	case Texture::Type::normal:
		material->AddTextures(std::vector<Texture*>{ newTexture });
		break;
	case Texture::Type::albedo:
		material->AddTextures(std::vector<Texture*>{ newTexture });
		break;
	case Texture::Type::roughness:
		roughness = newTexture;
		RefreshPBR();
		break;
	case Texture::Type::metallic:
		metallic = newTexture;
		RefreshPBR();
		break;
	case Texture::Type::PBR:
		break;
	case Texture::Type::ao:
		ao = newTexture;
		RefreshPBR();
		break;
	case Texture::Type::paint:
	case Texture::Type::emission:
	default:
		break;
	}
}

void ArtScene::ImportMesh(std::string& path, std::string& filename)
{
	// TODO: delete old model
	//model->meshes.clear();

	model = ResourceManager::LoadModel(path);

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

ArtScene::ArtScene()
{
	artScene = this;
}

void ArtScene::Start()
{
	model = ResourceManager::LoadModel();
	material = ResourceManager::LoadMaterial("New Material", shaders[super]);

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

	importTextures["roughness"] = &roughness;
	importTextures["metallic"] = &metallic;
	importTextures["ao"] = &ao;
}

void ArtScene::Update(float delta)
{
}

void ArtScene::Draw()
{
	renderSystem->Update(
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
		if (ImGui::CollapsingHeader("Current Material")) {
			ImGui::SliderFloat("Preview Scale", &texturePreviewScale, 0.01f, 1.0f, "% .3f", ImGuiSliderFlags_Logarithmic);

			ImGui::BeginChild("Textures", ImVec2(0, 0), ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_AlwaysHorizontalScrollbar);
			for (auto& i : importTextures)
			{
				ImGui::BeginGroup();
				ImGui::Text(i.first.c_str());
				if ((*i.second) != nullptr) {
					ImGui::Image((void*)(*i.second)->GLID, { texturePreviewScale * (float)(*i.second)->width, texturePreviewScale * (float)(*i.second)->height });
				}
				ImGui::EndGroup();
				ImGui::SameLine();
			}
			ImGui::BeginGroup();

			ImGui::Text("PBR");
			if ((pbr) != nullptr) {
				ImGui::Image((void*)pbr->GLID, { texturePreviewScale * (float)pbr->width, texturePreviewScale * (float)pbr->height });
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
}

ArtScene::~ArtScene()
{

}
