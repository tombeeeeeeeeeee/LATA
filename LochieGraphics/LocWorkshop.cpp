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
	animationTestPackets.emplace_back("sync1", Paths::importModelLocation + "ANIM_Sync_Run_BlockOut.dae");
	animationTestPackets.emplace_back("sync2", Paths::importModelLocation + "ANIM_Sync_Death_BlockOut.dae");
	animationTestPackets.emplace_back("sync3", Paths::importModelLocation + "ANIM_Sync_Idle_BlockOut.dae");
	animationTestPackets.emplace_back("sync4", Paths::importModelLocation + "ANIM_Sync_Shoot_Charge_BlockOut.dae");
	animationTestPackets.emplace_back("sync5", Paths::importModelLocation + "ANIM_Sync_Shoot_Hold_BlockOut.dae");
	animationTestPackets.emplace_back("sync6", Paths::importModelLocation + "ANIM_Sync_Shoot_Shot_BlockOut.dae");
	animationTestPackets.emplace_back("sync7", Paths::importModelLocation + "ANIM_Sync_Shoot_Snipe_BlockOut.dae");
	animationTestPackets.emplace_back("sync8", Paths::importModelLocation + "ANIM_Sync_Shoot_Charge.dae");
	animationTestPackets.emplace_back("sync9", Paths::importModelLocation + "ANIM_Sync_Shoot_Hold.dae");
	animationTestPackets.emplace_back("ecco1", Paths::importModelLocation + "SK_EccoLeanBack.dae");
	animationTestPackets.emplace_back("ecco2", Paths::importModelLocation + "SK_EccoLeanForward.dae");
	animationTestPackets.emplace_back("ecco3", Paths::importModelLocation + "SK_EccoLeanLeft.dae");
	animationTestPackets.emplace_back("ecco4", Paths::importModelLocation + "SK_EccoLeanRight.dae");
	//animationTestPackets.emplace_back("ecco", Paths::importModelLocation + "SK_EccoLeanForward.dae");

	

	glm::vec3 placePos = { 0.0f, 0.0f, 0.0f };
	for (auto& i : animationTestPackets)
	{
		i.sceneObject = new SceneObject(this, i.name);
		i.model = ResourceManager::LoadModel(i.path);
		i.sceneObject->setRenderer(new ModelRenderer(i.model, ResourceManager::defaultMaterial));
		i.animation = ResourceManager::LoadAnimation(i.path, i.model);
		i.sceneObject->setAnimator(new Animator(i.animation));

		i.sceneObject->transform()->setPosition(placePos);
		placePos += glm::vec3{ 200.0f, 0.0f, 0.0f };

		i.model->root.transform.setScale(1.0f);
	}

	Model* eccoModel = ResourceManager::LoadModel(Paths::importModelLocation + "SK_EccoLeanLeft.dae");
	eccoLeft = ResourceManager::LoadAnimation(Paths::importModelLocation + "SK_EccoLeanLeft.dae", eccoModel);

	//Model* eccoModel = ResourceManager::LoadModel(Paths::importModelLocation + "SK_EccoLeanRight.dae");
	eccoRight = ResourceManager::LoadAnimation(Paths::importModelLocation + "SK_EccoLeanRight.dae", eccoModel);

	//Model* eccoModel = ResourceManager::LoadModel(Paths::importModelLocation + "SK_EccoLeanForward.dae");
	eccoUp = ResourceManager::LoadAnimation(Paths::importModelLocation + "SK_EccoLeanForward.dae", eccoModel);

	//Model* eccoModel = ResourceManager::LoadModel(Paths::importModelLocation + "SK_EccoLeanBack.dae");
	eccoDown = ResourceManager::LoadAnimation(Paths::importModelLocation + "SK_EccoLeanBack.dae", eccoModel);

	eccoAnimator = Directional2dAnimator(eccoLeft, eccoRight, eccoUp, eccoDown);

	SceneObject* animatedEcco = new SceneObject(this, "animatedEcco");
	animatedEcco->setRenderer(new ModelRenderer(eccoModel, ResourceManager::defaultMaterial));
	animatedEcco->setAnimator(&eccoAnimator);

	animatedEcco->transform()->setPosition(placePos);

	eccoModel->root.transform.setScale(1.0f);



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
