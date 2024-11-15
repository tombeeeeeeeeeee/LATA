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
	camera->nearPlane = 10.0f;
	camera->farPlane = 50000.0f;
	camera->transform.setPosition({ 0.0f, 0.0f, 150.0f });
	camera->transform.setEulerRotation({ 0.0f, 90.0f, 0.0f });

	animationTestPackets.emplace_back("enemy1", Paths::importModelLocation + "SK_Idle(MeleeModel).fbx");
	animationTestPackets.emplace_back("enemy2", Paths::importModelLocation + "SK_Idle(RangeModel).fbx");
	animationTestPackets.emplace_back("enemy3", Paths::importModelLocation + "SK_Punch.fbx");
	animationTestPackets.emplace_back("enemy4", Paths::importModelLocation + "SK_Shoot.fbx");
	animationTestPackets.emplace_back("sync run blockout",         Paths::importModelLocation + "ANIM_Sync_Run_BlockOut.dae");
	animationTestPackets.emplace_back("sync run",                  Paths::importModelLocation + "ANIM_Sync_Run.dae");
	animationTestPackets.emplace_back("sync death blockout",       Paths::importModelLocation + "ANIM_Sync_Death_BlockOut.dae");
	animationTestPackets.emplace_back("sync death",                Paths::importModelLocation + "ANIM_Sync_Death.dae");
	animationTestPackets.emplace_back("sync idle blockout",        Paths::importModelLocation + "ANIM_Sync_Idle_BlockOut.dae");
	animationTestPackets.emplace_back("sync idle",                 Paths::importModelLocation + "ANIM_Sync_Idle.dae");
	animationTestPackets.emplace_back("sync charge blockout",      Paths::importModelLocation + "ANIM_Sync_Shoot_Charge_BlockOut.dae");
	animationTestPackets.emplace_back("sync charge",               Paths::importModelLocation + "ANIM_Sync_Shoot_Charge.dae");
	animationTestPackets.emplace_back("sync shoot hold blockout",  Paths::importModelLocation + "ANIM_Sync_Shoot_Hold_BlockOut.dae");
	animationTestPackets.emplace_back("sync shoot hold",           Paths::importModelLocation + "ANIM_Sync_Shoot_Hold.dae");
	animationTestPackets.emplace_back("sync shoot shot blockout",  Paths::importModelLocation + "ANIM_Sync_Shoot_Shot_BlockOut.dae");
	animationTestPackets.emplace_back("sync shoot shot",           Paths::importModelLocation + "ANIM_Sync_Shoot_Shot.dae");
	animationTestPackets.emplace_back("sync shoot snipe blockout", Paths::importModelLocation + "ANIM_Sync_Shoot_Snipe_BlockOut.dae");
	animationTestPackets.emplace_back("sync shoot snipe",          Paths::importModelLocation + "ANIM_Sync_Shoot_Snipe.dae");
	animationTestPackets.emplace_back("sync shoot charge",         Paths::importModelLocation + "ANIM_Sync_Shoot_Charge.dae");
	animationTestPackets.emplace_back("sync shoot hold",           Paths::importModelLocation + "ANIM_Sync_Shoot_Hold.dae");
	animationTestPackets.emplace_back("sync shoot snipe",          Paths::importModelLocation + "ANIM_Sync_Shoot_Snipe.dae");
	animationTestPackets.emplace_back("ecco lean back",            Paths::importModelLocation + "SM_EccoLeanBack.dae");
	animationTestPackets.emplace_back("ecco lead forward",         Paths::importModelLocation + "SM_EccoLeanForward.dae");
	animationTestPackets.emplace_back("ecco lean left",            Paths::importModelLocation + "SM_EccoLeanLeft.dae");
	animationTestPackets.emplace_back("ecco lean right",           Paths::importModelLocation + "SM_EccoLeanRight.dae");


	glm::vec3 placePos = { 0.0f, 0.0f, 0.0f };
	for (auto& i : animationTestPackets)
	{
		i.animatedSceneObject = new SceneObject(this, i.name);
		i.model = ResourceManager::LoadModel(i.path);
		i.animatedSceneObject->setRenderer(new ModelRenderer(i.model, ResourceManager::defaultMaterial));
		i.animation = ResourceManager::LoadAnimation(i.path, i.model);
		i.animatedSceneObject->setAnimator(new Animator(i.animation));

		i.animatedSceneObject->transform()->setPosition(placePos);

		i.modelSceneObject = new SceneObject(this, i.name + "_MODEL");
		i.modelSceneObject->setRenderer(new ModelRenderer(i.model, ResourceManager::defaultMaterial));
		i.modelSceneObject->transform()->setPosition(placePos + glm::vec3{ 0.0f, 0.0f, 600.0f });


		placePos += glm::vec3{ 400.0f, 0.0f, 0.0f };

		i.model->root.transform.setScale(1.0f);
	}

	//Model* eccoModel = ResourceManager::LoadModelAsset(Paths::modelSaveLocation + "SM_EccoLeanLeft.model");
	////eccoLeft = ResourceManager::LoadAnimation(Paths::importModelLocation + "SM_EccoLeanLeft.dae", eccoModel);

	////eccoRight = ResourceManager::LoadAnimation(Paths::importModelLocation + "SM_EccoLeanRight.dae", eccoModel);

	////eccoUp = ResourceManager::LoadAnimation(Paths::importModelLocation + "SM_EccoLeanForward.dae", eccoModel);

	////eccoDown = ResourceManager::LoadAnimation(Paths::importModelLocation + "SM_EccoLeanBack.dae", eccoModel);

	//eccoAnimator = Directional2dAnimator(eccoLeft, eccoRight, eccoUp, eccoDown);

	//SceneObject* animatedEcco = new SceneObject(this, "animatedEcco");
	//animatedEcco->setRenderer(new ModelRenderer(eccoModel, ResourceManager::defaultMaterial));
	//animatedEcco->setAnimator(&eccoAnimator);

	//animatedEcco->transform()->setPosition(placePos);

	//eccoModel->root.transform.setScale(1.0f);



	directionalLight.direction = glm::normalize(glm::vec3(-0.2f, -2.0f, 0.5f));
	directionalLight.colour = { 1.0f, 1.0f, 1.0f };
}

void LocWorkshop::Update(float delta)
{
}

void LocWorkshop::Draw(float delta)
{
	renderSystem.Update(
		renderers,
		transforms,
		renderers,
		animators,
		pointLights,
		spotlights,
		camera,
		delta,
		particleSystem.particles
	);
}

void LocWorkshop::GUI()
{
}


LocWorkshop::AnimationTestPacket::AnimationTestPacket(std::string _name, std::string _path) :
	name(_name),
	path(_path)
{

}
