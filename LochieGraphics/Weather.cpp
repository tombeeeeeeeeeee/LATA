#include "Weather.h"

#include <iostream>

Weather::Weather()
{
	std::srand(1337);
	//std::srand(700);

	for (auto i = map.Begin(); i < map.End(); i++)
	{
		i->windVelocity = glm::normalize(glm::vec2(rand() % 201 - 100.f,
			rand() % 201 - 100.f));
		if (glm::isnan(i->windVelocity.x)) {
			i->windVelocity = { 0.4, 0.4 };
		}
	}
	map(0, 0).windVelocity = { -1, -1 };
	map(2, 2).windVelocity = { -1, -1 };
	map(4, 4).windVelocity = { -1, -1 };
	map(6, 6).windVelocity = { -1, -1 };
	map(4, 6).windVelocity = { -1, -1 };
	map(4, 8).windVelocity = { -1, -1 };
}

void Weather::Update()
{
	glm::vec2 total(0, 0);
	for (auto i = map.Begin(); i != map.End(); i++)
	{
		total += i->windVelocity;
	}
	total /= map.getSize();
	std::cout << total.x << ", " << total.y << "\n";

	std::cout << glm::distance(total, glm::vec2(0, 0)) << "\n";
}
