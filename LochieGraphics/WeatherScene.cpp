#include "WeatherScene.h"

#include "Texture.h"

#include "ResourceManager.h";

#include <iostream>

void WeatherScene::Start()
{
	std::srand(1337);
	//std::srand(700);
	//vectorMap.resize(rows * collumns);
	
	for (auto i = vectorMap.Begin(); i < vectorMap.End(); i++)
	{
		//vectorMap[i].x = cosf(getRow(i) / (float)rows * 10) / 2.f + 0.5f;
		////vectorMap[i].y = getCol(i) / (float)collumns;
		//vectorMap[i].y = cosf(getCol(i) / (float)collumns * 10) / 2.f + 0.5f;
		//vectorMap[i] =  0.01f * (glm::vec2(rand() % 201 - 100.f, rand() % 201 - 100.f));
		//if (glm::isnan(vectorMap[i].x)) {
		//	vectorMap[i] = { 1, 0 };
		//}
		*i = glm::normalize(glm::vec2(rand() % 201 - 100.f, 
								rand() % 201 - 100.f));
		if (glm::isnan(i->x)) {
			*i = { 0.4, 0.4 };
		}
	}
	vectorMap(0, 0) = { -1, -1 };
	vectorMap(2, 2) = { -1, -1 };
	vectorMap(4, 4) = { -1, -1 };
	vectorMap(6, 6) = { -1, -1 };
	vectorMap(4, 6) = { -1, -1 };
	vectorMap(4, 8) = { -1, -1};
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
	glm::vec2 total(0, 0);
	for (auto i = vectorMap.Begin(); i != vectorMap.End(); i++)
	{
		total += *i;
	}
	total /= vectorMap.getSize();
	std::cout << total.x << ", " << total.y << "\n";

	std::cout << glm::distance(total, glm::vec2(0, 0)) << "\n";

	for (size_t i = 0; i < 100; i++)
	{
		Next();
	}
}

void WeatherScene::Next()
{
	Array2D<glm::vec2, 20, 20> oldMap = vectorMap;
	// This is averaging them atm, should obv not be like this
	for (int row = 0; row < vectorMap.getRowCount(); row++)
	{
		for (int col = 0; col < vectorMap.getColCount(); col++)
		{
			glm::vec2 total(0, 0);
			for (int r = -1; r < 2; r++)
			{
				for (int c = -1; c < 2; c++)
				{
					if (!r && !c) { continue; }
					total += oldMap(row + r, col + c);
				}
			}
			vectorMap(row, col) += total;
			vectorMap(row, col) /= 9.f;


			//glm::vec2 total(0, 0);
			//vectorMap(row - 1, col) += 

			//vectorMap(row, col) += total;
			//vectorMap(row, col) /= 9.f;	
		}
	}

	
}

void WeatherScene::SendToShader()
{
	int index = 0;
	for (auto i = vectorMap.Begin(); i != vectorMap.End(); i++)
	{
		shader->setVec2("vectorMap[" + std::to_string(index) + "]", *i);
		index++;
	}
}
