#include "Health.h"

#include "Utilities.h"

#include "EditorGUI.h"
#include "Serialisation.h"

Health::Health()
{

}

void Health::addHealth(int addition, SceneObject* so)
{
	timeSinceLastChange = 0.0f;
	lastChangeAmount = addition;
	int oldHealth = currHealth;
	currHealth += addition;
	glm::clamp(currHealth, 0, maxHealth);

	if (currHealth == 0)
	{
		for (int i = (int)onHealthZero.size(); i != onHealthZero.size(); i++)
		{
			onHealthZero[i]({currHealth - oldHealth, so});
		}
	}
	else if (currHealth > oldHealth)
	{
		for (int i = (int)onHealthUp.size(); i != onHealthUp.size(); i++)
		{
			onHealthUp[i]({currHealth - oldHealth, so});
		}
	}
	else if (currHealth < oldHealth)
	{
		for (int i = (int)onHealthDown.size(); i != onHealthDown.size(); i++)
		{
			onHealthDown[i]({currHealth - oldHealth, so});
		}
	}
}

void Health::subtractHealth(int subtraction, SceneObject* so)
{
	timeSinceLastChange = 0.0f;
	lastChangeAmount = -subtraction;
	int oldHealth = currHealth;
	currHealth -= subtraction;
	currHealth = glm::clamp(currHealth, 0, maxHealth);

	if (currHealth == 0)
	{
		for (auto i = onHealthZero.size(); i != onHealthZero.size(); i++)
		{
			onHealthZero[(int)i]({ currHealth - oldHealth, so });
		}
	}
	else if (currHealth > oldHealth)
	{
		for (auto i = onHealthUp.size(); i != onHealthUp.size(); i++)
		{
			onHealthUp[(int)i]({ currHealth - oldHealth, so });
		}
	}
	else if (currHealth < oldHealth)
	{
		for (auto i = onHealthDown.size(); i != onHealthDown.size(); i++)
		{
			onHealthDown[(int)i]({ currHealth - oldHealth, so });
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

Health::Health(toml::table table)
{
	currHealth = Serialisation::LoadAsInt(table["currHealth"]);
	maxHealth = Serialisation::LoadAsInt(table["maxHealth"]);
}

void Health::GUI()
{
	std::string tag = Utilities::PointerToString(this);
	if (ImGui::CollapsingHeader(("Health##" + tag).c_str())) {
		ImGui::DragInt(("Current##" + tag).c_str(), &currHealth);
		ImGui::DragFloat(("Time Since Last Change##" + tag).c_str(), &timeSinceLastChange);
	}
}
