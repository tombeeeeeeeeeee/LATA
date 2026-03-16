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

void EmptyScene::BeforeDraw(float delta)
{
	directionalLight = DirectionalLight
	({ 1.0f, 1.0f, 1.0f }, { -0.533f, -0.533f, -0.533f });
}

void EmptyScene::AfterDraw(float delta)
{
}

void EmptyScene::GUI()
{
}

EmptyScene::~EmptyScene()
{
}
