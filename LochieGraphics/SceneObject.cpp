#include "SceneObject.h"

#include "Shader.h"
#include "Model.h"
#include "ModelRenderer.h"

#include "imgui.h"
#include "imgui_stdlib.h"

SceneObject::SceneObject() :
	transform(Transform({ 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f }, 1.0f))
{
}

SceneObject::SceneObject(glm::vec3 _position, glm::vec3 _rotation, float _scale) :
	transform(Transform(_position, _rotation, _scale))
{
}

void SceneObject::setRenderer(ModelRenderer* modelRenderer)
{
	renderer = modelRenderer;
	renderer->sceneObject = this;
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

void SceneObject::Draw() const
{
	if (renderer) {
		renderer->Draw();
	}
	for (auto part = parts.begin(); part != parts.end(); part++)
	{
		(*part)->Draw();
	}
}

void SceneObject::GUI()
{
	ImGui::InputText("Name", &name);
	transform.GUI();
	renderer->GUI();
	for (auto i = parts.begin(); i != parts.end(); i++)
	{
		(*i)->GUI();
	}
}
