#pragma once

#include "UiElement.h"
#include "Input.h"
#include "StateMachine.h"
#include "SyncChargeLevel.h"

#include <string>

class Transform;
class RigidBody;
class LineRenderer;
class ModelRenderer;
class Shader;
class SceneObject;
struct Collision;

namespace toml {
	inline namespace v3 {
		class table;
	}
}

class Sync
{
public:
	unsigned long long GUID = 0;

	Sync();
	Sync(toml::table table);
	void Load(toml::table table);


	void Start(SceneObject* sceneObjectWithAnimator);

	bool Update(
		SceneObject* sceneObjectWithAnimator, Input::InputDevice& inputDevice, Transform& transform,
		RigidBody& rigidBody, LineRenderer* lines,
		float delta, float cameraAngleOffset, Transform* gunTransform
	);

	void GUI();

	toml::table Serialise() const;
	void ShootMisfire(glm::vec3 pos);
	void ShootSniper(glm::vec3 pos);
	void ShootOverClocked(glm::vec3 pos);
	void OverclockRebounding(glm::vec3 pos, glm::vec2 dir, int count, glm::vec3 colour);
	void OverclockRaindowShot(glm::vec3 pos, glm::vec2 dir, glm::vec3 colour, bool rebound);
	void misfireShotOnCollision(Collision collision);

	void LevelLoad();

	int currHealth = 5;
	int maxHealth = 100;

	float windowOfTimeForHealPressed = 0.2f;
	glm::vec3 globalBarrelOffset = {};
	glm::vec3 barrelOffset = { -80.0f, 70.0f, 5.0f };
	float shotWidth = 10.0f;

	float chargedDuration = 0.0f;
	float overclockChargeTime = 0.3f;


	UiElement chargeUI;
	UiElement healthUI;

	bool chargingShot = false;
	ChargeLevel lastShotLevel = ChargeLevel::none;

private:

	StateMachine animatorStateMachine;
	bool stateMachineSetup = false;


	std::vector<unsigned long long> misfireShots;

	float moveSpeed = 1.0f;
	float lookDeadZone = 0.03f;
	float moveDeadZone = 0.10f;
	
	int misfireDamage = 1;
	float misfireShotSpeed = 0.1f;

	int sniperDamage = 1;
	int sniperReboundCount = 0;
	float sniperChargeTime = 0.1f;
	float sniperBeamLifeSpan = 0.5f;
	glm::vec3 sniperBeamColour = { 0.5f,0.5f,1.0f };
	float knockBackForceSnipe = 10.0f;

	int overclockDamage = 1;
	float overclockBeamLifeSpan = 1.0f;
	glm::vec3 overclockBeamColour = { 1.0f,1.0f,0.5f };
	int enemyPierceCount = 99;
	int overclockReboundCount = 5;
	float knockBackForceOverclock = 10.0f;

	float eccoRefractionAngle = 30.0f;
	int eccoRefractionCount = 5;


	glm::vec2 fireDirection = {1.0f, 0.0f};

	float misfireColliderRadius = 0.1f;
	ModelRenderer* misfireModelRender = nullptr;
	std::string misfireModelPath = "SM_Sphere";
	std::string misfireMaterialPath;

	float timeSinceHealButtonPressed = 0.0f;

	float maxMoveForce = 1.0f;
	float maxStopForce = 1.0f;

	float stopSlowTime = 10.0f;
	float startSlowTime = 5.0f;

	bool rainbowRebounding = true;
	float rainbowDimming = 1.0f;
	int overclockBounceDamage = 10;
	
	bool reachedCharge1 = false;
	bool reachedCharge2 = false;
};
