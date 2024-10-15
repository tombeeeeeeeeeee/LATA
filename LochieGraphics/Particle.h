#pragma once

#include "Mesh.h"

#include "Maths.h"

#include <vector>

class Shader;
class ComputeShader;
class Texture;

class Particle
{
public:

	float quadSize = 10.0f; //  
	unsigned int count = 1000000;

	//Mesh mesh;
	unsigned int quadVAO = 0;
	unsigned int quadVBO = 0;
	Shader* shader = nullptr;
	ComputeShader* moveCompute = nullptr;
	ComputeShader* stopCompute = nullptr;
	ComputeShader* spreadCompute = nullptr;
	ComputeShader* resetCompute = nullptr;
	ComputeShader* explodeCompute = nullptr;

	Texture* texture;

	unsigned int ssbo;

	float explodeStrength = 1.0f;

	glm::mat4 model = glm::identity<glm::mat4>();

	void Spread();
	void Explode();
	void Stop();

	void Reset();


	void Initialise();
	void Update(float delta);
	void Draw();

	// TODO Rule of 
	~Particle();
};

