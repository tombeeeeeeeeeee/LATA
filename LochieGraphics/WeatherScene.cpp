#include "WeatherScene.h"

#include "Texture.h"

#include "ResourceManager.h";

#include <iostream>

void WeatherScene::Start()
{
	//auto data = ConvertToTextureData();
	quad.InitialiseQuad();

	//shader = ResourceManager::LoadShader("shaders/ui.vert", "shaders/ui.frag");
	shader = ResourceManager::LoadShader("shaders/weather.vert", "shaders/weather.frag");
}

void WeatherScene::Update(float delta)
{

	//texture->Bind(1);
	shader->Use();
	SendToShader();
	//shader->setSampler("vectorMap", 1);
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
