#pragma once
#include <unordered_map>
#include <vector>
#include "Maths.h"

class Health;
class ModelRender;
struct HealthPacket;

class HealthSystem
{
public:
	void Start(
		std::unordered_map<unsigned long long, Health>& healths
	);

	void Update(
		std::unordered_map<unsigned long long, Health>& healths,
		std::unordered_map<unsigned long long, ModelRenderer>& renderers,
		float delta
	);

private:
	void OnHealthDown(HealthPacket healthPacket);

public:
	glm::vec3 damageColour = { 0.8f,0.0f,0.0f };
	float colourTime = 0.5f;
};

