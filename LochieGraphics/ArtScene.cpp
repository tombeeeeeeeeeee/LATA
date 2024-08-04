#include "ArtScene.h"

#include "ResourceManager.h"
#include "SceneManager.h"

#include <filesystem>

ArtScene* ArtScene::artScene = nullptr;
Shader* ArtScene::singleChannelUIImage = nullptr;



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
		std::cout << "Unknown item dropped!: " << path << "\n";
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
	Texture* newTexture = ResourceManager::LoadTexture(path, type);

	texturePreviewScale = std::min((loadTargetPreviewSize / std::max(newTexture->width, newTexture->height)), texturePreviewScale);

	// Add texture to the material
	material->AddTextures(std::vector<Texture*>{ newTexture });
	std::cout << "Added texture: " << filename << "\n";

}

void ArtScene::ImportMesh(std::string& path, std::string& filename)
{
	model.meshes.clear();

	model = Model(path);
}

void ArtScene::ImportFolder(std::string& path)
{
	stringPaths.clear();
	newPaths.clear();
	for (const auto& entry : std::filesystem::directory_iterator(path)) {
		stringPaths.push_back(entry.path().string());
		newPaths.push_back(stringPaths.back().c_str());
	}
	ImportFromPaths(newPaths.size(), newPaths.data());
}

ArtScene::ArtScene()
{
	artScene = this;
}



void ArtScene::Start()
{
	model.AddMesh(new Mesh(Mesh::presets::cube));
	material = ResourceManager::LoadMaterial("New Material", shaders[super]);

	ModelRenderer* modelRenderer = new ModelRenderer(&model, material);
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
			for (auto& i : material->texturePointers)
			{
				ImGui::BeginGroup();
				ImGui::Text(i.first.c_str());
				if (i.second != nullptr) {
					switch (i.second->type)
					{
					case Texture::Type::height:
					case Texture::Type::PBR:
						//ImGui::GetWindowDrawList()->AddCallback(SetSingleChannelUIShader, (void*)(0));
						break;
					default:
						break;
					}
					ImGui::Image((void*)i.second->GLID, { texturePreviewScale * (float)i.second->width, texturePreviewScale * (float)i.second->height } );
					//ImGui::GetWindowDrawList()->AddCallback(SetToDefaultUIShader, (void*)(0));
				}
				ImGui::EndGroup();
				ImGui::SameLine();
			}
			ImGui::NewLine();
			ImGui::EndChild();
		}
	}
	ImGui::End();
}

ArtScene::~ArtScene()
{

}
