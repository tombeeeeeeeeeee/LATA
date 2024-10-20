#pragma once
#include "Scene.h"

#include "DirectionalLight.h"
#include "Animation.h"
#include "ComputeShader.h"
#include "Mesh.h"
#include "Particle.h"

class LocWorkshop : public Scene
{
private:

	DirectionalLight directionalLight = DirectionalLight
	({ 1.0f, 1.0f, 1.0f }, { -0.533f, -0.533f, -0.533f });

	SceneObject* sceneObject;
	SceneObject* aniTest;

	Animation animation;
	Animator* animator;
	
	Particle particle;

	Mesh* quad;
	Model* model;

	Shader* shader;
	Texture* texture;

public:

	LocWorkshop();
	void Start() override;
	void Update(float delta) override;
	void Draw() override;
	//	void OnMouseDown() override;
	void GUI() override;
	//	void OnWindowResize() override;
	//
	//~LocWorkshop() override;
};

