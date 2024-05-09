#pragma once

#include "Array2D.h"

#include "Maths.h"

class Weather
{
public:

	class Cell {
	public:
		glm::vec2 windVelocity;
		
	};

	Weather();
	void Update();
	
	Array2D<Cell, 100, 100> map;

};

