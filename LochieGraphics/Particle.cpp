#include "Particle.h"

#include "ComputeShader.h"
#include "Texture.h"

#include <chrono>
#include <iostream>

void Particle::Spread()
{
	spreadCompute->Run(cX, cY, cZ);
}

void Particle::Explode()
{
	explodeCompute->Run(cX, cY, cZ);
}

void Particle::Stop()
{
	stopCompute->Run(cX, cY, cZ);
}

void Particle::Reset()
{
	resetCompute->Run(cX, cY, cZ);
}

unsigned int Particle::getCount() const
{
	return cX * cY * cZ;
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


	// TODO: use Paths.h
	// TODO:
	//compute = new ComputeShader("")
	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);

	struct p {
		glm::vec4 pos;
		glm::vec4 vel;
	};

	std::vector<p> ps;
	ps.resize(getCount());
	for (size_t i = 0; i < getCount(); i++)
	{
		ps.at(i).pos = glm::vec4();
		ps.at(i).vel = glm::vec4();
	}

	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * getCount() * 2, &ps[0], GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Particle::Update(float delta)
{

	auto start = std::chrono::system_clock::now();

	moveCompute->Run(cX, cY, cZ);

	auto end = std::chrono::system_clock::now();

	std::chrono::duration<double> duration = end - start;
	std::cout << "Compute: " << duration.count() << '\n';
}

void Particle::Draw()
{
	auto start = std::chrono::system_clock::now();


	shader->Use();
	shader->setMat4("model", model);
	texture->Bind(1);
	shader->setSampler("material.albedo", 1);

	glBindVertexArray(quadVAO);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, getCount());

	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> duration = end - start;
	std::cout << "Draw: " << duration.count() << '\n';


}

Particle::~Particle()
{

}
