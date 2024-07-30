#include "ArtScene.h"

#include "ResourceManager.h"
#include "SceneManager.h"

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
		if (filename._Starts_with("T_")) 
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
				continue;
			}


			// Load texture
			Texture* newTexture = ResourceManager::LoadTexture(path, type);

			// Add texture to the material
			material->AddTextures(std::vector<Texture*>{ newTexture });
			std::cout << "Added texture!" << filename << "\n";
		}



	}
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
