#include "Weather.h"

#include <iostream>

Weather::Weather()
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

	for (size_t i = 0; i < 1; i++)
	{

		Array2D<glm::vec2, 20, 20> oldMap = vectorMap;
		// This is averaging them atm, should obv not be like this
		for (int row = 0; row < vectorMap.getRowCount(); row++)
		{
			for (int col = 0; col < vectorMap.getColCount(); col++)
			{
				//glm::vec2 total(0, 0);
				//for (int r = -1; r < 2; r++)
				//{
				//	for (int c = -1; c < 2; c++)
				//	{
				//		if (!r && !c) { continue; }
				//		total += oldMap(row + r, col + c);
				//	}
				//}
				//vectorMap(row, col) += total;
				//vectorMap(row, col) /= 9.f;



				//glm::vec2 slot = oldMap(row, col);

				//if (oldMap(row, col).x <.. 0) {
				//	vectorMap(row - 1, col).x += slot.x;
				//	vectorMap(row, col).x -= slot.x;
				//}
				//else {
				//	vectorMap(row + 1, col).x += oldMap(row, col).x;
				//	vectorMap(row, col).x -= oldMap(row, col).x;
				//}
				//if (oldMap(row, col).y < 0) {
				//	vectorMap(row, col - 1).y += oldMap(row, col).y;
				//	vectorMap(row, col).y -= oldMap(row, col).y;
				//}
				//else {
				//	vectorMap(row, col + 1).y += oldMap(row, col).y;
				//	vectorMap(row, col).y -= oldMap(row, col).y;
				//}
				//vectorMap(row, col) += total;
				//vectorMap(row, col) /= 9.f;	
			}
		}
	}

}
