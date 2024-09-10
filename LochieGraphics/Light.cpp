#include "Light.h"

#include "Utilities.h"

#include "ResourceManager.h"
#include "FrameBuffer.h"

#include "EditorGUI.h"

using Utilities::PointerToString;

Light::Light(glm::vec3 _colour) :
	colour(_colour)
{
}

void Light::Initialise()
{
	// Create shadow depth texture for the light
	depthMap = ResourceManager::LoadTexture(shadowTexWidth, shadowTexHeight, GL_DEPTH_COMPONENT, nullptr, GL_CLAMP_TO_BORDER, GL_FLOAT, false, GL_NEAREST, GL_NEAREST);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 }; // TODO: Move to be apart of texture
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	shadowFrameBuffer = new FrameBuffer(shadowTexWidth, shadowTexHeight, nullptr, depthMap, false);
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

	float previewScale = 0.1f;
	if (depthMap) {
		ImGui::Image((ImTextureID)(unsigned long long)depthMap->GLID, { previewScale * (float)shadowWidth, previewScale * (float)shadowHeight });
	}
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
