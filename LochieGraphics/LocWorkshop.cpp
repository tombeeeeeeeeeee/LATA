#include "LocWorkshop.h"

#include "Camera.h"
#include "SceneObject.h"
#include "ResourceManager.h"
#include "Paths.h"
#include "ComputeShader.h"
#include "Animation.h"

#include "EditorGUI.h"
#include "ExtraEditorGUI.h"

#include <iostream>

LocWorkshop::LocWorkshop()
{
}

void LocWorkshop::Start()
{
	lights.insert(lights.end(), { &directionalLight });

	camera->nearPlane = 10.0f;
	camera->farPlane = 50000.0f;
	camera->transform.setPosition({ 0.0f, 0.0f, 150.0f });
	camera->transform.setEulerRotation({ 0.0f, 90.0f, 0.0f });

	SceneObject* enemy1 = new SceneObject(this, "enemy1");
	SceneObject* enemy2 = new SceneObject(this, "enemy2");
	SceneObject* enemy3 = new SceneObject(this, "enemy3");
	SceneObject* enemy4 = new SceneObject(this, "enemy4");
	std::string path1 = Paths::importModelLocation + "SK_Idle(MeleeModel).fbx";
	std::string path2 = Paths::importModelLocation + "SK_Idle(RangeModel).fbx";
	std::string path3 = Paths::importModelLocation + "SK_Punch.fbx";
	std::string path4 = Paths::importModelLocation + "SK_Shoot.fbx";
	Model* model1 = ResourceManager::LoadModel(path1);
	Model* model2 = ResourceManager::LoadModel(path2);
	Model* model3 = ResourceManager::LoadModel(path3);
	Model* model4 = ResourceManager::LoadModel(path4);
	enemy1->setRenderer(new ModelRenderer(model1, ResourceManager::defaultMaterial));
	enemy2->setRenderer(new ModelRenderer(model2, ResourceManager::defaultMaterial));
	enemy3->setRenderer(new ModelRenderer(model3, ResourceManager::defaultMaterial));
	enemy4->setRenderer(new ModelRenderer(model4, ResourceManager::defaultMaterial));
	animation1 = Animation(path1, model1);
	animation2 = Animation(path2, model2);
	animation3 = Animation(path3, model3);
	animation4 = Animation(path4, model4);
	enemy1->setAnimator(new Animator(&animation1));
	enemy2->setAnimator(new Animator(&animation2));
	enemy3->setAnimator(new Animator(&animation3));
	enemy4->setAnimator(new Animator(&animation4));
}

void LocWorkshop::Update(float delta)
{
}

void LocWorkshop::Draw()
{
	renderSystem.Update(
		renderers,
		transforms,
		renderers,
		animators,
		camera,
		particleSystem.particles
	);
}

void LocWorkshop::GUI()
{
}
