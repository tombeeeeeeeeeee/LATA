#include "TestScene.h"

#include "ResourceManager.h"

TestScene::TestScene()
{
}


void TestScene::Start()
{
	testTexture = ResourceManager::defaultNormal;


	overlayShader = ResourceManager::LoadShader("Shaders/default.vert", "Shaders/simpleTexturedWithCutout.frag");

	quad.InitialiseQuad(1.0f);
}

void TestScene::Update(float delta)
{

}

void TestScene::Draw(float delta)
{

	renderSystem.Update(
		transforms,
		pointLights,
		spotlights,
		camera,
		delta
	);


	overlayShader->Use();
	overlayShader->setFloat("material.alpha", 1.0f);

	testTexture->Bind(1);
	overlayShader->setSampler("material.albedo", 1);

	quad.Draw();

}

void TestScene::GUI()
{
}

TestScene::~TestScene()
{
}
