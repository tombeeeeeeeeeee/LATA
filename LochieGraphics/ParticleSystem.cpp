#include "ParticleSystem.h"

#include "ResourceManager.h";

#include "EditorGUI.h"

void ParticleSystem::Initialise(Shader* _shader)
{
	shader = _shader;
	nextParticleTexture = ResourceManager::defaultTexture;
}

void ParticleSystem::Update(float delta)
{
	// Note: If this container type changes might have to change how they are getting deleted
	std::vector<std::vector<Particle*>::iterator> toDelete;
	for (auto i = particles.begin(); i != particles.end(); i++)
	{
		(*i)->Update(delta);
		if ((*i)->lifetime < 0) {
			toDelete.push_back(i);
		}
	}

	// TODO: There is prob a better way to do this
	// Deleting backwards to avoid invaliding iterators
	for (auto i = toDelete.rbegin(); i != toDelete.rend(); i++)
	{
		delete** i;
		particles.erase(*i);
	}
}

void ParticleSystem::Draw(std::vector<Particle*>& particles)
{
	for (auto i : particles)
	{
		i->shader->Use();
		i->Draw();
	}
}

void ParticleSystem::GUI()
{
	for (size_t i = 0; i < particles.size(); i++)
	{
		if (ImGui::CollapsingHeader(("P" + std::to_string(i)).c_str())) {
			ImGui::Indent();
			particles.at(i)->GUI();
			ImGui::Unindent();
		}
	}

	ImGui::DragInt("Count", &nextParticleCount, 1.0f, 1, INT_MAX);

	ImGui::DragFloat("Lifetime", &nextParticleLifetime);

	ImGui::DragFloat3("Position", &nextStartingPos.x);

	ResourceManager::TextureSelector("Texture", &nextParticleTexture, false);

	if (ImGui::Button("Add Another Particles")) {
		particles.emplace_back(new Particle(nextParticleCount, nextParticleLifetime, shader, nextParticleTexture, nextStartingPos));
		particles.back()->Initialise();
	}
}
