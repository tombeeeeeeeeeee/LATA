#include "PointLight.h"

#include "Shader.h"

#include "Utilities.h"

#include "EditorGUI.h"
#include "Serialisation.h"



PointLight::PointLight(toml::table table)
{
	quadratic = Serialisation::LoadAsFloat(table["quadratic"]);
	linear = Serialisation::LoadAsFloat(table["linear"]);
	range = Serialisation::LoadAsFloat(table["range"]);
	colour = Serialisation::LoadAsVec3(table["colour"]);
	triggerTag = Serialisation::LoadAsString(table["triggerTag"]);
	canBeTriggered = Serialisation::LoadAsBool(table["canBeTriggered"]);
	on = Serialisation::LoadAsBool(table["on"]);
	effect = (PointLightEffect)Serialisation::LoadAsInt(table["effect"]);
	intensity = Serialisation::LoadAsFloat(table["intensity"], 1.0f);
}

void PointLight::GUI()
{
	std::string tag = Utilities::PointerToString(this);
	if (ImGui::CollapsingHeader(("Point Light##" + tag).c_str()))
	{
		ImGui::Indent();
		std::string tag = Utilities::PointerToString(this);
		ImGui::Checkbox(("On##" + tag).c_str(), &on);

		ImGui::ColorPicker3(("Light Colour##" + tag).c_str(), &colour[0], ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR);
		float _range = 100.0f * range;
		ImGui::DragFloat("Intensity", &intensity);
		if (ImGui::DragFloat(("Range##" + tag).c_str(), &_range, 0.1f))
			SetRange(_range);

		const char* effects[] = {
			"On", "Off", "Flickering", "SyncGun"
		};
		const char* currType = effects[(int)effect];
		ImGui::PushItemWidth(180);
		if (ImGui::BeginCombo(("Effect Type##" + tag).c_str(), currType))
		{
			for (int i = 0; i < 4; i++)
			{
				bool isSelected = i == (int)effect;
				if (ImGui::Selectable(effects[i], isSelected))
				{
					effect = (PointLightEffect)i;
					timeInType = 0.0f;
				}
				if (isSelected) ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		ImGui::InputText(("Trigger ID##" + tag).c_str(), &triggerTag);
		ImGui::Checkbox(("Can Be Triggered##" + tag).c_str(), &canBeTriggered);
		ImGui::Unindent();
	}
}


void PointLight::SetRange(float _range)
{
	range = _range/100.0f;

	if (range <= 7)
	{
		linear = 1 - range * 0.3f / 7;
		quadratic = 1.8f + (7 - range) * 0.5f;
	}

	else if (range <= 13)
	{
		float ratio = range - (13 - 7);
		ratio /= 7;
		linear = 0.7f + (0.35f - 0.7f) * ratio;
		quadratic = 1.8f + (0.44f - 1.8f) * ratio;
	}

	else if (range <= 20)
	{
		float ratio = range - (20 - 13);
		ratio /= 13;
		linear = 0.35f + (0.22f - 0.35f) * ratio;
		quadratic = 0.44f + (0.2f - 0.44f) * ratio;
	}

	else if (range <= 32)
	{
		float ratio = range - (32 - 20);
		ratio /= 20;
		linear = 0.22f + (0.14f - 0.22f) * ratio;
		quadratic = 0.2f + (0.07f - 0.2f) * ratio;
	}

	else if (range <= 50)
	{
		float ratio = range - (50 - 32);
		ratio /= 32;
		linear = 0.14f + (0.09f - 0.14f) * ratio;
		quadratic = 0.07f + (0.032f - 0.07f) * ratio;
	}

	else if (range <= 65)
	{
		float ratio = range - (65 - 50);
		ratio /= 50;
		linear = 0.09f + (0.07f - 0.09f) * ratio;
		quadratic = 0.032f + (0.017f - 0.032f) * ratio;
	}

	else if (range <= 100)
	{
		float ratio = range - (100 - 65);
		ratio /= 65;
		linear = 0.07f + (0.045f - 0.07f) * ratio;
		quadratic = 0.017f + (0.0075f - 0.017f) * ratio;
	}

	else if (range <= 160)
	{
		float ratio = range - (160 - 100);
		ratio /= 100;
		linear = 0.045f + (0.027f - 0.045f) * ratio;
		quadratic = 0.0075f + (0.0028f - 0.0075f) * ratio;
	}
	else if (range <= 200)
	{
		float ratio = range - (200 - 160);
		ratio /= 160;
		linear = 0.027f + (0.022f - 0.027f) * ratio;
		quadratic = 0.0028f + (0.0019f - 0.0028f) * ratio;
	}
	else if (range <= 325)
	{
		float ratio = range - (325 - 200);
		ratio /= 200;
		linear = 0.022f + (0.014f - 0.022f) * ratio;
		quadratic = 0.0019f + (0.0007f - 0.0019f) * ratio;
	}
	else if (range <= 600)
	{
		float ratio = range - (600 - 325);
		ratio /= 325;

		linear = 0.014f + (0.007f - 0.014f) * ratio;
		quadratic = 0.0007f + (0.0002f - 0.0007f) * ratio;
	}
	else
	{
		float ratio = range - (3250 - 600);
		ratio /= 600;
		linear = 0.007f + (0.0014f - 0.007f) * ratio;
		quadratic = 0.0002f + (0.000007f - 0.0002f) * ratio;
	}
}

void PointLight::SetRange(float _linear, float _quadratic)
{

	if (_quadratic >= 1.8f)
	{
		range = 7;
	}

	else if (quadratic >= 0.44f)
	{
		float ratio = _quadratic - (0.44f - 1.8f);
		ratio /= -1.8f;
		range = 7 + (13 - 7) * ratio;
	}

	else if (range <= 20)
	{
		float ratio = _quadratic - (0.2f - 0.44f);
		ratio /= -0.44f;
		range = 13 + (20 - 13) * ratio;
	}

	else if (range <= 32)
	{
		float ratio = _quadratic - (0.07f - 0.2f);
		ratio /= -0.2f;
		range = 20 + (32 - 20) * ratio;
	}

	else if (range <= 50)
	{
		float ratio = _quadratic - (0.032f - 0.07f);
		ratio /= -0.07f;
		range = 32 + (50 - 32) * ratio;
	}

	else if (range <= 65)
	{
		float ratio = _quadratic - (0.017f - 0.032f);
		ratio /= -0.032f;
		range = 50 + (65 - 50) * ratio;
	}

	else if (range <= 100)
	{
		float ratio = _quadratic - (0.0075f - 0.017f);
		ratio /= -0.017f;
		range = 65 + (100 - 65) * ratio;
	}

	else if (range <= 160)
	{
		float ratio = _quadratic - (0.0028f - 0.0075f);
		ratio /= -0.0075f;
		range = 100 + (160 - 100) * ratio;
	}
	else if (range <= 200)
	{
		float ratio = _quadratic - (0.0019f - 0.0028f);
		ratio /= -0.0028f;
		range = 160 + (200 - 160) * ratio;
	}
	else if (range <= 325)
	{
		float ratio = _quadratic - (0.0007f - 0.0019f);
		ratio /= -0.0019f;
		range = 200 + (325 - 200) * ratio;
	}
	else if (range <= 600)
	{
		float ratio = _quadratic - (0.0002f - 0.0007f);
		ratio /= -0.0007f;
		range = 325 + (600 - 325) * ratio;
	}
	else
	{
		float ratio = _quadratic - (0.000007f - 0.0002f);
		ratio /= -0.0002f;
		range = 600 + (3250 - 600) * ratio;
	}
}

toml::table PointLight::Serialise(unsigned long long guid) const
{
	return toml::table{
		{"guid", Serialisation::SaveAsUnsignedLongLong(guid)},
		{ "quadratic", quadratic },
		{ "linear", linear },
		{ "range", range },
		{ "colour", Serialisation::SaveAsVec3(colour) },
		{ "triggerTag", triggerTag },
		{ "on", on },
		{ "canBeTriggered", canBeTriggered },
		{ "effect", (int)effect },
		{ "intensity", intensity },
	};
}

void PointLight::TriggerCall(std::string tag, bool toggle)
{
	if (canBeTriggered)
	{
		if (tag == triggerTag)
		{
			if (toggle && on) return;
			if (!toggle && !on) return;

			on = toggle;
			if (toggle) effect = PointLightEffect::On;
			else effect = PointLightEffect::Off;
		}
	}
}
