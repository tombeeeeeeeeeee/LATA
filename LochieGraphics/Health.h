#pragma once

#include "Serialisation.h"

#include <functional>
#include <vector>

class Health
{
public:
	int currHealth = 0;

	void addHealth(int addition);
	void subtractHealth(int subtraction);

	void setMaxHealth(int max);
	int getMaxHealth();

	toml::table Serialise(unsigned long long GUID) const;

private:
	int maxHealth = 0;
	std::vector<std::function<void(int)>> onHealthUp = {};
	std::vector<std::function<void(int)>> onHealthDown = {};
	std::vector<std::function<void(int)>> onHealthZero = {};
};

