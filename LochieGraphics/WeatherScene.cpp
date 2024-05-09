#include "WeatherScene.h"

#include "ResourceManager.h";

#include <iostream>

void WeatherScene::Start()
{
	quad.InitialiseQuad(0.009f);

	shader = ResourceManager::LoadShader("shaders/weather.vert", "shaders/weather.frag");
}

void WeatherScene::Update(float delta)
{
	
}

void WeatherScene::Draw()
{
	shader->Use();

	for (size_t c = 0; c < weather.map.getColCount() - 1; c++)
	{
		for (size_t r = 0; r < weather.map.getRowCount() - 1; r++)
		{
			Weather::Cell cell = weather.map(r, c);
			
			shader->setVec2("wind", cell.windVelocity);

			shader->setVec2("pos", r * 0.009f - 0.50f, c * 0.009f - 0.5f);

			quad.Draw();
		}
	}
}

void WeatherScene::OnMouseDown()
{
	weather.Update();
}
