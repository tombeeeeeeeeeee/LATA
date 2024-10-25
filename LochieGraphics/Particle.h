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
	bool initialised = false;

	float quadSize = 10.0f; //  

	unsigned int count;
	float lifetime;

	std::vector<glm::vec4> positions;
	std::vector<glm::vec4> velocities;

	// TODO: Switch with a proper mesh
	unsigned int quadVAO = 0;
	unsigned int quadVBO = 0;
	Shader* shader = nullptr;

	Texture* texture;

	static std::random_device random;

	unsigned int ssbo = 0;

	float explodeStrength = 1.0f;

	glm::mat4 model = glm::identity<glm::mat4>();

	Particle(unsigned int _count, float _lifetime, Shader* _shader, Texture* _texture, glm::vec3 startingPos);

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

