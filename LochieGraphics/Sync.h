#pragma once
#include "Input.h"
#include <string>
#include "Maths.h"
#include "BlastLine.h"

class Transform;
class RigidBody;
class LineRenderer;
class ModelRenderer;
class Shader;
struct Collision;

class Sync
{
public:
	unsigned long long GUID = 0;

	Sync() {};

	void Start(std::vector<Shader*>* shaders);

	void Update(
		Input::InputDevice& inputDevice,
		Transform& transform,
		RigidBody& rigidBody,
		LineRenderer* lines,
		float delta	
	);

	void GUI();

	void ShootMisfire(Transform& transform);
	void ShootSniper(glm::vec3 pos);
	void ShootOverClocked(glm::vec3 pos);
	void OverclockedRebounding(glm::vec3 pos, glm::vec2 dir, int count, glm::vec3 colour);

	void misfireShotOnCollision(Collision collision);
	void sniperShotOnCollision(Collision collision);
	void overclockShotOnCollision(Collision collision);

private:

	std::vector<unsigned long long> misfireShots;
	std::vector<BlastLine> blasts;

	float moveSpeed = 1.0f;
	float lookDeadZone = 0.03f;
	float moveDeadZone = 0.10f;

	glm::vec3 barrelOffset = { 0.0f, 0.0f, 0.0f };

	float misfireDamage = 0.0f;
	float misfireChargeCost = 0.0f;
	float misfireShotSpeed = 0.1f;

	float sniperDamage = 0.0f;
	float sniperChargeCost = 0.0f;
	float sniperChargeTime = 0.0f;
	float sniperBeamLifeSpan = 0.5f;
	glm::vec3 sniperBeamColour = { 1.0f,1.0f,1.0f };

	float overclockDamage = 0.0f;
	float overclockChargeCost = 0.0f;
	float overclockChargeTime = 0.0f;
	float overclockBeamLifeSpan = 1.0f;
	glm::vec3 overclockBeamColour = { 1.0f,1.0f,1.0f };
	int enemyPierceCount = 99;
	int overclockReboundCount = 5;

	float eccoRefractionAngle = 10.0f;
	int eccoRefractionCount = 5;

	float chargedDuration = 0.0f;
	bool chargingShot = false;
	float currCharge = 100000.0f;
	float maxCharge = 100000.0f;

	glm::vec2 fireDirection = {1.0f, 0.0f};

	float misfireColliderRadius = 0.1f;
	ModelRenderer* misfireModelRender = nullptr;
	std::string misfireModelPath;
	std::string misfireMaterialPath;

};
