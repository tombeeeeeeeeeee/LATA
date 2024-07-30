#include "ArtScene.h"

#include "ResourceManager.h"

ArtScene::ArtScene()
{
}

void ArtScene::Start()
{
	model.AddMesh(new Mesh(Mesh::presets::cube));
	Material* material = ResourceManager::LoadMaterial("New Material", shaders[super]);

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
