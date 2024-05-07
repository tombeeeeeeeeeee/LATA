#pragma once
#include "SceneObject.h"

#include "Transform.h"
#include "ModelRenderer.h"

#include "Serialisation.h"

#include "Maths.h"

#include <vector>
#include <string>

class Model;
class Shader;

// TODO: Make version of the SceneObject that draws the same model in multiple spots, using instancing
class SceneObject
{
private:
	ModelRenderer* renderer = nullptr;
public:

	std::string name = "Unnamed Scene Object";

	Transform transform;

	std::vector<Part*> parts;

	SceneObject();
	SceneObject(glm::vec3 _position, glm::vec3 _rotation = { 0.f, 0.f, 0.f }, float _scale = 1.0f);

	void setRenderer(ModelRenderer* modelRenderer);
	ModelRenderer* getRenderer() const;

	~SceneObject();

	SceneObject(const SceneObject& other) = delete;
	SceneObject& operator=(const SceneObject& other) = delete;

	void Update(float delta);
	void Draw(Shader* override = nullptr) const;
	void GUI();

	toml::table Serialise();
};



