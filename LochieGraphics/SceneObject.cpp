#include "SceneObject.h"

#include "Shader.h"
#include "Model.h"
#include "ModelRenderer.h"

#include "imgui.h"
#include "imgui_stdlib.h"

SceneObject::SceneObject() :
	transform(Transform(this))
{
}

SceneObject::SceneObject(glm::vec3 _position, glm::vec3 _rotation, float _scale) :
	transform(Transform(this, _position, _rotation, _scale))
{
}

void SceneObject::setRenderer(ModelRenderer* modelRenderer)
{
	if (renderer != nullptr) { return; }
	renderer = modelRenderer;
	renderer->sceneObject = this;
	parts.push_back(modelRenderer);
}

ModelRenderer* SceneObject::getRenderer() const
{
	return renderer;
}

SceneObject::~SceneObject()
{
	delete renderer;
}

void SceneObject::Update(float delta)
{
	for (auto part = parts.begin(); part != parts.end(); part++)
	{
		(*part)->Update();
	}
}

void SceneObject::Draw(Shader* override) const
{
	for (auto part = parts.begin(); part != parts.end(); part++)
	{
		(*part)->Draw(override);
	}
}

void SceneObject::GUI()
{
	ImGui::InputText("Name", &name);
	transform.GUI();
	for (auto i = parts.begin(); i != parts.end(); i++)
	{
		(*i)->GUI();
	}
}

toml::table SceneObject::Serialise()
{
	return toml::table{
		{ "name", name },
		{ "hasRenderer", renderer != nullptr}
	};
}
