#include "WeatherScene.h"

#include "ResourceManager.h";

#include <iostream>

void WeatherScene::Start()
{
	quad.InitialiseQuad();

	shader = ResourceManager::LoadShader("shaders/weather.vert", "shaders/weather.frag");
}

void WeatherScene::Update(float delta)
{
	shader->Use();
	SendToShader();
	quad.Draw();
}

void WeatherScene::OnMouseDown()
{
	weather.Update();
}

void WeatherScene::SendToShader()
{
	int index = 0;
	for (auto i = weather.vectorMap.Begin(); i != weather.vectorMap.End(); i++)
	{
		shader->setVec2("vectorMap[" + std::to_string(index) + "]", *i);
		index++;
	}
}
