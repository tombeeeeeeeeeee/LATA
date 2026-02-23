#include "Health.h"

#include "Utilities.h"

#include "EditorGUI.h"
#include "Serialisation.h"

float Health::cooldown = 0.0f;

Health::Health()
{

}

void Health::addHealth(int addition)
{
	timeSinceLastChange = 0.0f;
	lastChangeAmount = addition;
	currHealth += addition;
	currHealth = glm::clamp(currHealth, 0, maxHealth);
}

bool Health::subtractHealth(int subtraction)
{
	if (timeSinceLastChange > cooldown)
	{
		timeSinceLastChange = 0.0f;
		lastChangeAmount = -subtraction;
		currHealth -= subtraction;
		currHealth = glm::clamp(currHealth, 0, maxHealth);
		return true;
	}
	return false;
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
		ImGui::DragInt(("Max Health##" + tag).c_str(), &maxHealth);
	}
}
