#pragma once

#include "Mesh.h"

#include "Maths.h"

#include <random>

class Shader;
class ComputeShader;
class Texture;

class Particle
{
public:
	bool initialised = false;

	float quadSize = 10.0f; //  

	unsigned int count;
	float lifetime = 0.0f;
	float lifeSpan = 0.0f;
	float sizeStart = 1.0f;
	float sizeEnd = 1.0f;
	float gravity = 0.0f;

	std::vector<glm::vec4> positions;
	std::vector<glm::vec4> velocities;
	std::vector<float> scales;

	// TODO: Switch with a proper mesh
	unsigned int quadVAO = 0;
	unsigned int quadVBO = 0;
	Shader* shader = nullptr;

	Texture* texture = nullptr;
	Texture* colourOverTime = nullptr;

	static std::random_device random;

	unsigned int ssbo = 0;

	float explodeStrength = 1.0f;

	glm::vec3 colour = { 1.0f, 1.0f, 1.0f };
	glm::vec4 velEncouragement = { 0.0f, 0.0f, 0.0f , 0.0f};
	glm::mat4 model;

	Particle(unsigned int _count, float _lifetime, Shader* _shader, Texture* _texture, glm::vec3 startingPos, float scaleDelta = 0.0f);

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

