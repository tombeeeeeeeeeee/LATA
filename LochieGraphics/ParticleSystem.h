#pragma once

#include "Particle.h"

class Shader;

class ParticleSystem
{
public:
	Texture* nextParticleTexture;
	int nextParticleCount = 10;
	float nextParticleLifetime = 4.0f;
	glm::vec3 nextStartingPos = { 0.0f, 0.0f, 0.0f };

	Shader* shader;

	std::vector<Particle*> particles = {};

	void Initialise(Shader* _shader);

	void Update(float delta);

	static void Draw(std::vector<Particle*>& particles);

	// TODO: Rename
	Particle* AddParticle(unsigned int _count, float _lifetime, Texture* _texture, glm::vec3 startingPos);

	void GUI();
};

