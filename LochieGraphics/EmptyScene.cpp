#include "EmptyScene.h"

EmptyScene::EmptyScene()
{
}

void EmptyScene::Start()
{

}

void EmptyScene::Update(float delta)
{

}

void EmptyScene::Draw(float delta)
{
	directionalLight = DirectionalLight
	({ 1.0f, 1.0f, 1.0f }, { -0.533f, -0.533f, -0.533f });

	renderSystem.Update(
		renderers,
		transforms,
		renderers,
		animators,
		pointLights,
		spotlights,
		camera,
		delta
	);
}

void EmptyScene::GUI()
{
}

EmptyScene::~EmptyScene()
{
}
