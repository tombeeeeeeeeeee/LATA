#include "Particle.h"

#include "Shader.h"

void Particle::Explode()
{
	for (auto& i : velocities)
	{
		std::uniform_real_distribution<float> distribution(-10, 10);

		i += explodeStrength * (glm::normalize(glm::vec3{ distribution(random), distribution(random), distribution(random) }) * 2.0f - 1.0f);
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
	mesh.InitialiseQuad(quadSize);
	positions.resize(count);
	velocities.resize(count);
}

void Particle::Update(float delta)
{
	//for (size_t i = 0; i < count; i++)
	//{
	//	positions.at(i) += glm::vec3(i * delta, i * 2 * delta, i * 3 * delta);
	//}
	for (size_t i = 0; i < count; i++)
	{
		positions.at(i) += velocities.at(i);
	}
}

void Particle::Draw()
{
	shader->Use();
	shader->setMat4("model", model);

	for (size_t i = 0; i < count; i++)
	{
		shader->setVec3("positions[" + std::to_string(i) + "]", positions[i]);
	}
	
	for (size_t i = 0; i < count; i++)
	{
		shader->setVec3("position", positions.at(i));
		glBindVertexArray(mesh.getVAO());
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, count);

		//mesh.Draw();
	}
}
