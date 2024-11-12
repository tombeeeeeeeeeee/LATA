#include "Spotlight.h"

#include "Graphics.h"
#include "Utilities.h"

#include "EditorGUI.h"
#include "Serialisation.h"

int Spotlight::SHADOW_DIMENSIONS = 1024;

Spotlight::Spotlight()
{
	Initialise();
}

Spotlight::Spotlight(toml::table table) : PointLight::PointLight(table)
{
	direction = Serialisation::LoadAsVec3(table["direction"], {1.0f,0.0f,0.0f});
	cutOff = Serialisation::LoadAsFloat(table["cutOff"]);
	outerCutOff = Serialisation::LoadAsFloat(table["outerCutOff"]);
	castsShadows = Serialisation::LoadAsBool(table["castsShadows"]);
	Initialise();
}

Spotlight::~Spotlight()
{
	glDeleteTextures(1, &depthBuffer);
	glDeleteFramebuffers(1, &frameBuffer);
}


Spotlight::Spotlight(Spotlight&& other)
{
	/*Copy the actual boring variables*/
	linear = other.linear;
	quadratic = other.quadratic;
	range = other.range;
	on = other.on;
	canBeTriggered = other.canBeTriggered;
	triggerTag = other.triggerTag;
	colour = other.colour;
	effect = other.effect;
	direction = other.direction;
	cutOff = other.cutOff;
	outerCutOff = other.outerCutOff;
	castsShadows = other.castsShadows;
	intensity = other.intensity;

	depthBuffer = other.depthBuffer;
	other.depthBuffer = 0;
	frameBuffer = other.frameBuffer;
	other.frameBuffer = 0;
}

Spotlight& Spotlight::operator=(Spotlight&& other)
{
	if (this == &other) return *this;

	/*Copy the actual boring variables*/
	linear = other.linear;
	quadratic = other.quadratic;
	range = other.range;
	on = other.on;
	canBeTriggered = other.canBeTriggered;
	triggerTag = other.triggerTag;
	colour = other.colour;
	effect = other.effect;
	direction = other.direction;
	cutOff = other.cutOff;
	outerCutOff = other.outerCutOff;
	castsShadows = other.castsShadows;
	intensity = other.intensity;

	glDeleteTextures(1, &depthBuffer);
	glDeleteFramebuffers(1, &frameBuffer);


	depthBuffer = other.depthBuffer;
	other.depthBuffer = 0;
	frameBuffer = other.frameBuffer;
	other.frameBuffer = 0;

	return *this;
}

void Spotlight::Initialise()
{
	glGenTextures(1, &depthBuffer);
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glBindTexture(GL_TEXTURE_2D, depthBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, SHADOW_DIMENSIONS, SHADOW_DIMENSIONS, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBuffer, 0);
}

void Spotlight::GUI()
{
	std::string tag = Utilities::PointerToString(this);
	if (ImGui::CollapsingHeader(("Spotlight##" + tag).c_str()))
	{
		ImGui::Indent();
		std::string tag = Utilities::PointerToString(this);
		if (ImGui::Checkbox(("On##" + tag).c_str(), &on))
		{
			if (on)
			{
				effect = PointLightEffect::On;
			}
			else
			{
				effect = PointLightEffect::Off;
			}
			timeInType = 0;
		};
		ImGui::ColorPicker3(("Colour##" + tag).c_str(), &colour[0], ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR);
		//Tom Changed this, feel free to set it back.
		float _range = 100.0f * range;
		if (ImGui::DragFloat(("Range##" + tag).c_str(), &_range, 0.1f))
			SetRange(_range);

		ImGui::DragFloat("Intensity", &intensity);
		ImGui::SliderFloat(("Cut off##" + tag).c_str(), &cutOff, 0.f, 1.f);
		ImGui::SliderFloat(("Outer cut off##" + tag).c_str(), &outerCutOff, 0.f, 1.f);
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
		if (ImGui::DragFloat3(("Direction" + tag).c_str(), &direction[0]))
		{
			direction = glm::normalize(direction);
		}
		ImGui::InputText(("Trigger ID##" + tag).c_str(), &triggerTag);
		ImGui::Checkbox(("Can Be Triggered##" + tag).c_str(), &canBeTriggered);
		ImGui::Checkbox(("Casts Shadows##" + tag).c_str(), &castsShadows);
		ImGui::Unindent();
	}
}

glm::mat4 Spotlight::getProj()
{
	return glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, range * 500.0f);
}

glm::mat4 Spotlight::getView(glm::mat4 globalTransform)
{
	glm::vec3 pos = globalTransform[3];
	glm::vec3 up = globalTransform * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	glm::vec3 globalDir = globalTransform * glm::vec4(direction, 0.0f);
	return glm::lookAt(pos, pos + globalDir, up);
}

toml::table Spotlight::Serialise(unsigned long long guid) const
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
		{ "direction",Serialisation::SaveAsVec3(direction) },
		{ "cutOff", cutOff },
		{ "outerCutOff", outerCutOff },
		{ "castsShadows", castsShadows },
	};
}


