#include "ArtScene.h"

#include "ResourceManager.h"
#include "SceneManager.h"

#include "stb_image.h"
#include "stb_image_write.h"

#include <filesystem>

ArtScene* ArtScene::artScene = nullptr;
Shader* ArtScene::singleChannelUIImage = nullptr;



void ArtScene::RefreshPBR()
{
	if (pbr != nullptr) {
		pbr->DeleteTexture();
	}

	if (metallic == nullptr || roughness == nullptr) {
		return;
	}

	// 4 channels
	std::vector<unsigned char> data(base->width * base->height * 4);

	int tempW;
	int tempH;
	int tempC;
	unsigned char* metallicData = stbi_load(metallic->path.c_str(), &tempW, &tempH, &tempC, STBI_default);
	if (tempW != base->width || tempH != tempH) { return; }
	unsigned char* roughnessData = stbi_load(roughness->path.c_str(), &tempW, &tempH, &tempC, STBI_default);
	if (tempW != base->width || tempH != tempH) { return; }
	unsigned char* ambientData;
	if (ao) {
		ambientData = stbi_load(ao->path.c_str(), &tempW, &tempH, &tempC, STBI_default);
	}
	else {
		ambientData = {};
	}
	

	for (int i = 0; i < base->width * base->height; i++)
	{
		int col = i % base->width;
		int row = i / base->height;

		data[i * 4 + 0] = metallicData[i];
		data[i * 4 + 1] = roughnessData[i];
		if (ao) {
			data[i * 4 + 2] = ambientData[i];
		}
		else {
			data[i * 4 + 2] = (unsigned char)255;
		}
		data[i * 4 + 3] = (unsigned char)255;

	}


	pbr = ResourceManager::LoadTexture(base->width, base->height, GL_SRGB_ALPHA, data.data(), GL_REPEAT, GL_UNSIGNED_BYTE, true);

	material->AddTextures({ pbr });

	// TODO: Get name from base image
	int result = stbi_write_tga("./newPBR.tga", base->width, base->height, STBI_rgb_alpha, data.data());

	std::cout << "Wrote PBR image, with result of " << result << '\n';
}

void ArtScene::SetSingleChannelUIShader(const ImDrawList* parent_list, const ImDrawCmd* cmd)
{
	glGetIntegerv(GL_CURRENT_PROGRAM, &artScene->defaultUIShader);

	singleChannelUIImage->Use();
}

void ArtScene::SetToDefaultUIShader(const ImDrawList* parent_list, const ImDrawCmd* cmd)
{
	glUseProgram(artScene->defaultUIShader);
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
		// Find last of will be -1 if none found (when cast to an int)
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

	Texture* newTexture = ResourceManager::LoadTexture(path, type, GL_REPEAT, defaultFlip);

	texturePreviewScale = std::min((loadTargetPreviewSize / std::max(newTexture->width, newTexture->height)), texturePreviewScale);

	switch (type)
	{
	case Texture::Type::normal:
		normal = newTexture;
		material->AddTextures(std::vector<Texture*>{ newTexture });
		break;
	case Texture::Type::emission:
		break;
	case Texture::Type::albedo:
		base = newTexture;
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
		break;
	default:
		break;
	}
}

void ArtScene::ImportMesh(std::string& path, std::string& filename)
{
	// TODO: delete old model
	//model->meshes.clear();

	model = ResourceManager::LoadModel(path);

	sceneObject->renderer()->Refresh();
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
	//model = ResourceManager::LoadModel("models/Beauty Corner/SM_BeautyCorner.fbx");
	model = ResourceManager::LoadModel();
	//model->AddMesh(Mesh(Mesh::presets::cube));
	material = ResourceManager::LoadMaterial("New Material", shaders[super]);
	//material->AddTextures({
	//	//ResourceManager::LoadTexture("models/Beauty Corner/T_MetalAssets_BaseColour.tga", Texture::Type::albedo, )
	//	});

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

	singleChannelUIImage = ResourceManager::LoadShader("ui");

	importTextures["base"] = &base;
	importTextures["roughness"] = &roughness;
	importTextures["normal"] = &normal;
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
					//ImGui::GetWindowDrawList()->AddCallback(SetSingleChannelUIShader, (void*)(0));
					ImGui::Image((void*)(*i.second)->GLID, { texturePreviewScale * (float)(*i.second)->width, texturePreviewScale * (float)(*i.second)->height });
					//ImGui::GetWindowDrawList()->AddCallback(SetToDefaultUIShader, (void*)(0));
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
	}
	ImGui::End();
}

ArtScene::~ArtScene()
{

}
