#include "Health.h"
#include "Maths.h"

void Health::addHealth(int addition)
{
	int oldHealth = currHealth;
	currHealth += addition;
	glm::clamp(currHealth, 0, maxHealth);

	if (currHealth == 0)
	{
		for (int i = onHealthZero.size(); i != onHealthZero.size(); i++)
		{
			onHealthZero[i](currHealth);
		}
	}
	else if (currHealth > oldHealth)
	{
		for (int i = onHealthUp.size(); i != onHealthUp.size(); i++)
		{
			onHealthUp[i](currHealth);
		}
	}
	else if (currHealth < oldHealth)
	{
		for (int i = onHealthDown.size(); i != onHealthDown.size(); i++)
		{
			onHealthDown[i](currHealth);
		}
	}
}

void Health::subtractHealth(int subtraction)
{
	int oldHealth = currHealth;
	currHealth -= subtraction;
	glm::clamp(currHealth, 0, maxHealth);

	if (currHealth == 0)
	{
		for (int i = onHealthZero.size(); i != onHealthZero.size(); i++)
		{
			onHealthZero[i](currHealth);
		}
	}
	else if (currHealth > oldHealth)
	{
		for (int i = onHealthUp.size(); i != onHealthUp.size(); i++)
		{
			onHealthUp[i](currHealth);
		}
	}
	else if (currHealth < oldHealth)
	{
		for (int i = onHealthDown.size(); i != onHealthDown.size(); i++)
		{
			onHealthDown[i](currHealth);
		}
	}
}

void Health::setMaxHealth(int max)
{
	maxHealth = max;
}

int Health::getMaxHealth()
{
	return maxHealth;
}

toml::table Health::Serialise(unsigned long long GUID) const
{
	// TODO: Ensure the function pointers are all good
	return toml::table {
		{ "guid", Serialisation::SaveAsUnsignedLongLong(GUID) },
		{ "currHealth", currHealth },
		{ "maxHealth", maxHealth }
	};
}
