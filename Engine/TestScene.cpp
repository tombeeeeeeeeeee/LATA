#include "TestScene.h"

#include "SceneObject.h"

TestScene::TestScene()
{
}

void TestScene::Start()
{
}

void TestScene::Update(float delta)
{
}

void TestScene::Draw(float delta)
{
	renderSystem.Update(
		renderers,
		transforms,
		renderers,
		animators,
		pointLights,
		spotlights,
		decals,
		shadowWalls,
		camera,
		delta
	);
}

void TestScene::OnMouseDown()
{

}

void TestScene::GUI()
{
}


void TestScene::OnWindowResize()
{
}

TestScene::~TestScene()
{
}
