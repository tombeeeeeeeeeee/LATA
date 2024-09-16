#pragma once

#include "Serialisation.h"

#include <functional>
#include <vector>

class SceneObject;

struct HealthPacket
{
	int healthChange;
	SceneObject* so;

	HealthPacket(int _healthChange, SceneObject* _so = nullptr) : healthChange(_healthChange), so(_so) {};
};

class Health
{
public:
	int currHealth = 0;
	float timeSinceLastChange = 0.0f;
	Health();

	void addHealth(int addition, SceneObject* so = nullptr);
	void subtractHealth(int subtraction, SceneObject* so = nullptr);

	void setMaxHealth(int max);
	int getMaxHealth();

	toml::table Serialise(unsigned long long GUID) const;
	Health(toml::table table);

	std::vector<std::function<void(HealthPacket)>> onHealthUp = {};
	std::vector<std::function<void(HealthPacket)>> onHealthDown = {};
	std::vector<std::function<void(HealthPacket)>> onHealthZero = {};

private:
	int maxHealth = 0;

};

