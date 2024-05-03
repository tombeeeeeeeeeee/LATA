#pragma once

#include "Array2D.h"

#include "Maths.h"

class Weather
{
public:

	Weather();
	void Update();
	
	Array2D<glm::vec2, 20, 20> vectorMap;

};

