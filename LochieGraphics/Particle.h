#pragma once

#include "Mesh.h"

#include "Maths.h"

#include <vector>
#include <random>

class Shader;

class Particle
{
public:

	float quadSize = 10.0f; //  
	const unsigned int count = 1000000;
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> velocities;

	//Mesh mesh;
	unsigned int quadVAO = 0;
	unsigned int quadVBO = 0;
	Shader* shader = nullptr;

	unsigned int instanceVBO;

	std::random_device random;

	float explodeStrength = 1.0f;

	glm::mat4 model = glm::identity<glm::mat4>();

	void Spread();
	void Explode();

	void Reset();

	void Initialise();
	void Update(float delta);
	void Draw();
};

