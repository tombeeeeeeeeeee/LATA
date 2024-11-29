#include "Particle.h"

#include "ComputeShader.h"
#include "Texture.h"

#include "ExtraEditorGUI.h"

#include <chrono>
#include <iostream>

std::random_device Particle::random = {};

Particle::Particle(unsigned int _count, float _lifetime, Shader* _shader, Texture* _texture, glm::vec3 startingPos) :
	count(_count),
	lifetime(_lifetime),
	shader(_shader),
	texture(_texture),
	initialised(false)
{
	positions.assign(count, glm::vec4(startingPos.x, startingPos.y, startingPos.z, 0.0f));
	velocities.resize(count);
	glm::quat rot = glm::quat(glm::vec3(45.5f, 215.0f, 0.0f)  * PI / 180.0f);
	model = glm::mat4_cast(rot);
}

void Particle::Spread()
{
	for (size_t i = 0; i < count; i++)
	{
		positions.at(i) += glm::vec4{ i, i, i, 0 };
	}
}

void Particle::Explode()
{
	for (auto& i : velocities)
	{
		std::uniform_real_distribution<float> distribution(-10.0f, 10.0f);

		i += explodeStrength * glm::normalize(glm::vec4{ distribution(random), distribution(random), distribution(random), 0.0f }) * 0.5f;
		if (glm::isnan(i.x)) {
			i = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
		}
	}
	for (auto& i : velocities)
	{
		std::uniform_real_distribution<float> distribution(-10.0f, 10.0f);

		i += explodeStrength * glm::normalize(glm::vec4{ distribution(random), distribution(random), distribution(random), 0.0f }) * 0.5f;
		if (glm::isnan(i.x)) {
			i = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
		}
	}
}

void Particle::Stop()
{
	for (size_t i = 0; i < count; i++)
	{
		velocities.at(i) = { 0.0f, 0.0f, 0.0f, 0.0f };
	}
}

void Particle::Reset()
{
	for (auto& i : positions)
	{
		i = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	for (auto& i : velocities)
	{
		i = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	}
}

unsigned int Particle::getCount() const
{
	return count;
}

void Particle::Initialise()
{	
	//mesh.InitialiseQuad(quadSize);
	float size = quadSize;
	float quadVerts[] = {
		 size,  size, 0.0f  ,   0.0f, 0.0f,  1.0f  ,   1.0f, 1.0f,
		-size,  size, 0.0f  ,   0.0f, 0.0f,  1.0f  ,   0.0f, 1.0f,
		-size, -size, 0.0f  ,   0.0f, 0.0f,  1.0f  ,   0.0f, 0.0f,
		 size,  size, 0.0f  ,   0.0f, 0.0f,  1.0f  ,   1.0f, 1.0f,
		-size, -size, 0.0f  ,   0.0f, 0.0f,  1.0f  ,   0.0f, 0.0f,
		 size, -size, 0.0f  ,   0.0f, 0.0f,  1.0f  ,   1.0f, 0.0f
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

	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);


	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * 4 * getCount(), &positions[0], GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	initialised = true;
}

void Particle::Update(float delta)
{
	for (size_t i = 0; i < count; i++)
	{
		positions.at(i) += velocities.at(i);
	}
	lifetime -= delta;
}

void Particle::Draw()
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssbo);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(float) * 4 * getCount(), &positions[0]);

	shader->Use();
	
	shader->setMat4("model", model);
	texture->Bind(1);
	shader->setSampler("material.albedo", 1);
	shader->setFloat("lifeTime", lifetime);
	shader->setVec3("material.colour", colour);

	glBindVertexArray(quadVAO);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, getCount());

}

void Particle::GUI()
{
	std::string tag = Utilities::PointerToString(this);
	ImGui::DragFloat(("Explode Strength##" + tag).c_str(), &explodeStrength);

	if (ImGui::Button(("Reset##Particles" + tag).c_str())) {
		Reset();
	}

	if (ImGui::Button(("Explode##Particles" + tag).c_str())) {
		Explode();
	}

	if (ImGui::Button(("Initialise##Particles" + tag).c_str())) {
		Initialise();
	}

	if (ImGui::Button(("Spread##Particles" + tag).c_str())) {
		Spread();
	}

	if (ImGui::Button(("Stop##Particle" + tag).c_str())) {
		Stop();
	}

	//ImGui::Checkbox("Facing Camera##Particle", &renderSystem.particleFacingCamera);

	//ImGui::DragScalar("Count##Particle", ImGuiDataType_U32, reinterpret_cast<void*>(&particle.count));

	ExtraEditorGUI::Mat4Input(("Model Matrix##Particle" + tag).c_str(), &model);
}

Particle::~Particle()
{
	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &quadVBO);
	glDeleteBuffers(1, &ssbo);
}
