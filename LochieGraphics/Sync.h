#pragma once
#include "Input.h"
#include <string>
#include "Maths.h"
#include "Serialisation.h"

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
	Sync(toml::table table);

	void Start(std::vector<Shader*>* shaders);

	void Update(
		Input::InputDevice& inputDevice, Transform& transform,
		RigidBody& rigidBody, LineRenderer* lines,
		float delta, float cameraAngleOffset
	);

	void GUI();

	toml::table Serialise() const;
	void ShootMisfire(Transform& transform);
	void ShootSniper(glm::vec3 pos);
	void ShootOverClocked(glm::vec3 pos);
	void OverclockRebounding(glm::vec3 pos, glm::vec2 dir, int count, glm::vec3 colour);
	void OverclockNonRebounding(glm::vec3 pos, glm::vec2 dir, glm::vec3 colour);
	void misfireShotOnCollision(Collision collision);

private:

	std::vector<unsigned long long> misfireShots;
	std::vector<BlastLine> blasts;

	float moveSpeed = 1.0f;
	float lookDeadZone = 0.03f;
	float moveDeadZone = 0.10f;

	glm::vec3 barrelOffset = { 0.0f, 0.0f, 0.0f };

	int misfireDamage = 0;
	float misfireChargeCost = 0.0f;
	float misfireShotSpeed = 0.1f;

	int sniperDamage = 0;
	float sniperChargeCost = 0.0f;
	float sniperChargeTime = -0.1f;
	float sniperBeamLifeSpan = 0.5f;
	glm::vec3 sniperBeamColour = { 0.5f,0.5f,1.0f };

	int overclockDamage = 0;
	float overclockChargeCost = 0.0f;
	float overclockChargeTime = 0.3f;
	float overclockBeamLifeSpan = 1.0f;
	glm::vec3 overclockBeamColour = { 1.0f,1.0f,0.5f };
	int enemyPierceCount = 99;
	int overclockReboundCount = 5;

	float eccoRefractionAngle = 1.0f;
	int eccoRefractionCount = 360;

	float chargedDuration = 0.0f;
	bool chargingShot = false;
	float currCharge = 100000.0f;
	float maxCharge = 100000.0f;

	glm::vec2 fireDirection = {1.0f, 0.0f};

	float misfireColliderRadius = 0.1f;
	ModelRenderer* misfireModelRender = nullptr;
	std::string misfireModelPath = "SM_Sphere";
	std::string misfireMaterialPath;

};
