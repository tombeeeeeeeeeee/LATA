#pragma once
#include "Scene.h"

#include "Lights.h"
#include "MessengerInterface.h"
#include "Animator.h"
#include "BlendedAnimator.h"
#include "Animation.h"

class TestScene : public Scene
{
private:
	MessengerInterface messengerInterface;

	Shader* uiShader = nullptr;

	int skyboxIndex = 0;
	std::vector<Skybox*> skyboxes;

	SceneObject* backpack;
	SceneObject* soulSpear;
	SceneObject* testRedBox;
	SceneObject* grass;
	SceneObject* boxes;
	SceneObject* lightCube;
	SceneObject* xbot;
	SceneObject* vampire;
	SceneObject* aniTest;
	SceneObject* tires;

	Animation* xbotChicken;
	Animation* xbotIdle;
	Animator xbotAnimator;
	Animator xbotOtherAnimator;
	BlendedAnimator xbotBlendedAnimator;
	float lerpAmount = 0.f;

	Animation* vampireWalk;
	Animator vampireAnimator;

	


public:

	TestScene();
	void Start() override;
	void Update(float delta) override;
	void Draw(float delta) override;
	void OnMouseDown() override;
	void GUI() override;
	void OnWindowResize() override;

	~TestScene() override;
};