#include "ArtScene.h"

#include "ResourceManager.h"
#include "SceneManager.h"

#include <filesystem>

ArtScene* ArtScene::artScene = nullptr;

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
		else {
			int lastSlash = path.find_last_of("/\\");
			int lastPeriod = path.find_last_of(".");
			if (lastPeriod == std::string::npos) {
				lastPeriod = 0;
			}
			if (lastSlash > lastPeriod) {
				stringPaths.clear();
				newPaths.clear();
				for (const auto& entry : std::filesystem::directory_iterator(path)) {
					stringPaths.push_back(entry.path().string());
					newPaths.push_back(stringPaths.back().c_str());
				}
				ImportFromPaths(newPaths.size(), newPaths.data());
				continue;
			}
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

	// Add texture to the material
	material->AddTextures(std::vector<Texture*>{ newTexture });
	std::cout << "Added texture!" << filename << "\n";

}

void ArtScene::ImportMesh(std::string& path, std::string& filename)
{
	model.meshes.clear();

	model = Model(path);
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



ArtScene::~ArtScene()
{
}
