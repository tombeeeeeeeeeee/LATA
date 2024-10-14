#include "Particle.h"

#include "Shader.h"

void Particle::Spread()
{
	for (size_t i = 0; i < count; i++)
	{
		positions.at(i) += glm::vec3{ i, i, i };
	}
}

void Particle::Explode()
{
	for (auto& i : velocities)
	{
		std::uniform_real_distribution<float> distribution(-10.0f, 10.0f);

		i += explodeStrength * glm::normalize(glm::vec3{ distribution(random), distribution(random), distribution(random) });
		if (glm::isnan(i.x)) {
			//
			i = { 1.0f, 0.0f, 0.0f };
		}
	}
}

void Particle::Reset()
{
	for (auto& i : positions)
	{
		i = glm::vec3(0.0f, 0.0f, 0.0f);
	}
	for (auto& i : velocities)
	{
		i = glm::vec3(0.0f, 0.0f, 0.0f);
	}
}

void Particle::Initialise()
{
	positions.resize(count);
	velocities.resize(count);

	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * count, &positions[0], GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//mesh.InitialiseQuad(quadSize);
	float size = quadSize;
	float quadVerts[] = {
		 size ,  size, 0.0f  ,   0.0f, 0.0f,  1.0f  ,   1.0f, 1.0f,
		- size,  size, 0.0f  ,   0.0f, 0.0f,  1.0f  ,   0.0f, 1.0f,
		- size, -size, 0.0f  ,   0.0f, 0.0f,  1.0f  ,   0.0f, 0.0f,
		 size ,  size, 0.0f  ,   0.0f, 0.0f,  1.0f  ,   1.0f, 1.0f,
		- size, -size, 0.0f  ,   0.0f, 0.0f,  1.0f  ,   0.0f, 0.0f,
		 size , -size, 0.0f  ,   0.0f, 0.0f,  1.0f  ,   1.0f, 0.0f
	};

	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);

	// Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	// Normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	// Texture
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));


	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO); // this attribute comes from a different vertex buffer

	// TODO: GL_DYNAMIC_DRAW or GL_STREAM_DRAW?
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * count, &positions[0], GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glVertexAttribDivisor(3, 1);



}

void Particle::Update(float delta)
{
	for (size_t i = 0; i < count; i++)
	{
		positions.at(i) += velocities.at(i);
	}

}

void Particle::Draw()
{
	shader->Use();
	shader->setMat4("model", model);

	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * count, &positions[0]);

	glBindVertexArray(quadVAO);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, count); // 100 triangles of 6 vertices each

}
