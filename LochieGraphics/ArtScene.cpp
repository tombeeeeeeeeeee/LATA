#include "ArtScene.h"

#include "ResourceManager.h"
#include "SceneManager.h"

#include "imgui_stdlib.h"

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

	bool foundImage = false;

	for (auto& i : importImages)
	{
		if (!i.second->loaded) { continue; }
		if (foundImage) {
			if (width != i.second->width || height != i.second->height) {
				std::cout << "Mismatched PBR texture sizes " << i.second->path << " does not match with the other last given texture\n"
					<< "this can be ignored if in process of inputting textures\n";
				return;
			}
		}
		width = i.second->width;
		height = i.second->height;
		foundImage = true;
	}


	if (width == 0 || height == 0) {
		return;
	}
	unsigned int size = (unsigned int)width * (unsigned int)height;
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

	pbr = ResourceManager::LoadTexture(width, height, GL_SRGB_ALPHA, data.data(), GL_REPEAT, GL_UNSIGNED_BYTE, true);
	pbr->type = Texture::Type::PBR;
	material->AddTextures({ pbr });

	// TODO: Get name from base image
	int result = stbi_write_tga("./newPBR.tga", width, height, STBI_rgb_alpha, data.data());

	int tW, tH, tC;
	unsigned char* test = stbi_load("./newPBR.tga", &tW, &tH, &tC, STBI_rgb_alpha);

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
			(*importTextures.at(i.first))->Load(i.second->data);
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

	Texture* newTexture;

	switch (type)
	{
	case Texture::Type::albedo: case Texture::Type::normal: case Texture::Type::emission:
		newTexture = ResourceManager::LoadTexture(path, type, GL_REPEAT, defaultFlip);
		material->AddTextures(std::vector<Texture*>{ ResourceManager::LoadTexture(path, type, GL_REPEAT, defaultFlip) });
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

	case Texture::Type::PBR:
		break;
	case Texture::Type::paint: default:
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
	metallicPreview->format = GL_RED;
	roughnessPreview = ResourceManager::LoadTexture(1024, 1024);
	roughnessPreview->format = GL_RED;
	aoPreview = ResourceManager::LoadTexture(1024, 1024);
	aoPreview->format = GL_RED;

	metallicImage.components = 1;
	roughnessImage.components = 1;
	aoImage.components = 1;

	importTextures["metallic"] = &metallicPreview;
	importTextures["roughness"] = &roughnessPreview;
	importTextures["ao"] = &aoPreview;

	importImages["metallic"] = &metallicImage;
	importImages["roughness"] = &roughnessImage;
	importImages["ao"] = &aoImage;
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


		if (ImGui::InputText("Filter##ArtStuff", &filter)) {
			filteredMaterials.clear();
			auto& materials = ResourceManager::getMaterials();
			for (auto& i : materials)
			{
				std::string name = i.second.name + " " + std::to_string(i.second.GUID);
				if ((name).find(filter) != std::string::npos) {
					filteredMaterials.push_back(std::pair<std::string, const Material*>{ name, &i.second});
				}
			}
		}
		
		for (auto& i : filteredMaterials)
		{
			ImGui::Text(i.first.c_str());
		}




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
