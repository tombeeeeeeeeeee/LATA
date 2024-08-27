#include "EmptyScene.h"

EmptyScene::EmptyScene()
{
}

void EmptyScene::Start()
{
	lights.insert(lights.end(), {
	&directionalLight,
	&spotlight,
	&pointLights[0],
	&pointLights[1],
	&pointLights[2],
	&pointLights[3],
	});
}

void EmptyScene::Update(float delta)
{

}

void EmptyScene::Draw()
{
	renderSystem->Update(
		renderers,
		transforms,
		renderers,
		animators,
		camera
	);
}

void EmptyScene::GUI()
{
}

EmptyScene::~EmptyScene()
{
}
