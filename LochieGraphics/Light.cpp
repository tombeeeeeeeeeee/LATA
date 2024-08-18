#include "Light.h"

#include "Utilities.h"

#include "imgui.h"

using Utilities::PointerToString;

Light::Light(glm::vec3 _colour) :
	colour(_colour)
{
}

glm::mat4 Light::getShadowViewProjection() const
{
	return getShadowProjection() * getShadowView();
}

void Light::GUI()
{
	std::string tag = PointerToString(this);
	//CHECK:
	ImGui::ColorEdit3(("Colour##" + tag).c_str(), &colour[0], ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float);

	ImGui::Text("Shadows Planes");
	ImGui::DragFloat(("Near plane##" + tag).c_str(), &shadowNearPlane, 0.01f, 0.01f, FLT_MAX);
	ImGui::DragFloat(("Far plane##" + tag).c_str(), &shadowFarPlane, 0.01f, 0.01f, FLT_MAX);

	ImGui::BeginDisabled();
	int shadowWidth = shadowTexWidth;
	int shadowHeight = shadowTexHeight;
	ImGui::DragInt("Shadow Texture Width", &shadowWidth, 1, 0, INT_MAX);
	ImGui::DragInt("Shadow Texture Height", &shadowHeight, 1, 0, INT_MAX);
	// TODO: Set width height
	ImGui::EndDisabled();
}

toml::table Light::Serialise() const
{
	Type type = getType();
	return toml::table{
		{ "type", (int)type },
		{"colour", Serialisation::SaveAsVec3(colour)},
		{"near", shadowNearPlane},
		{"shadowWidth", shadowTexWidth},
		{"shadowHeight", shadowTexHeight},
		// TODO shadowFrameBuffer
	};
}
