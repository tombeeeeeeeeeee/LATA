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

	aniTest = new SceneObject(this, "aniTest");
	std::string path = Paths::importModelLocation + "Anim_Sync_RunTEST.fbx";
	model = ResourceManager::LoadModel(path);
	aniTest->setRenderer(new ModelRenderer(model, ResourceManager::defaultMaterial));
	animation = Animation(path, model);
	aniTest->setAnimator(new Animator(&animation));
	animator = aniTest->animator();
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
