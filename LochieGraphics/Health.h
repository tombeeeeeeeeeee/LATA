#pragma once

#include <functional>
#include <vector>

class SceneObject;

namespace toml { inline namespace v3 { 
	class table; 
} }

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
	float timeSinceLastChange = 1.0f;
	int lastChangeAmount = 0.0f;
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

	void GUI();

private:
	int maxHealth = 0;

};

