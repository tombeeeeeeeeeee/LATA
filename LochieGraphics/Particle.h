#pragma once

#include "Mesh.h"

#include "Maths.h"

#include <random>
#include <vector>

class Shader;
class ComputeShader;
class Texture;

class Particle
{
public:

	float quadSize = 10.0f; //  

	const unsigned int count = 100000;
	std::vector<glm::vec4> positions;
	std::vector<glm::vec4> velocities;

	//Mesh mesh;
	unsigned int quadVAO = 0;
	unsigned int quadVBO = 0;
	Shader* shader = nullptr;

	Texture* texture;

	static std::random_device random;

	unsigned int ssbo;

	float explodeStrength = 1.0f;

	glm::mat4 model = glm::identity<glm::mat4>();

	Particle() = default;

	void Spread();
	void Explode();
	void Stop();

	void Reset();

	unsigned int getCount() const;

	void Initialise();
	void Update(float delta);
	void Draw();

	void GUI();

	// TODO Rule of 
	~Particle();
};

