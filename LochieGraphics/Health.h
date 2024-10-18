#pragma once

#include <functional>
#include <vector>

class SceneObject;

namespace toml { inline namespace v3 { 
	class table; 
} }


class Health
{
public:
	int currHealth = 0;
	float timeSinceLastChange = 1.0f;
	int lastChangeAmount = 0;
	Health();

	void addHealth(int addition);
	void subtractHealth(int subtraction);

	void setMaxHealth(int max);
	int getMaxHealth();

	toml::table Serialise(unsigned long long GUID) const;
	Health(toml::table table);

	void GUI();

private:
	int maxHealth = 0;

};

