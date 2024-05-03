#include "Weather.h"

#include <iostream>

Weather::Weather()
{
	std::srand(1337);
	//std::srand(700);

	for (auto i = vectorMap.Begin(); i < vectorMap.End(); i++)
	{
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
	vectorMap(4, 8) = { -1, -1 };
}

void Weather::Update()
{
	glm::vec2 total(0, 0);
	for (auto i = vectorMap.Begin(); i != vectorMap.End(); i++)
	{
		total += *i;
	}
	total /= vectorMap.getSize();
	std::cout << total.x << ", " << total.y << "\n";

	std::cout << glm::distance(total, glm::vec2(0, 0)) << "\n";
}
