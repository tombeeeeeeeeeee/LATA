#pragma once
#include "BlastLine.h"
#include "Input.h"

#include <string>

class Transform;
class RigidBody;
class LineRenderer;
class ModelRenderer;
class Shader;
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
	void ShootMisfire(glm::vec3 pos);
	void ShootSniper(glm::vec3 pos);
	void ShootOverClocked(glm::vec3 pos);
	void OverclockRebounding(glm::vec3 pos, glm::vec2 dir, int count, glm::vec3 colour);
	void OverclockNonRebounding(glm::vec3 pos, glm::vec2 dir, glm::vec3 colour);
	void misfireShotOnCollision(Collision collision);

	int currHealth = 5;
	int maxHealth = 5;
	glm::vec3 globalBarrelOffset = {};
	glm::vec3 barrelOffset = { -80.0f, 70.0f, 5.0f };
private:

	std::vector<unsigned long long> misfireShots;
	std::vector<BlastLine> blasts;

	float moveSpeed = 1.0f;
	float lookDeadZone = 0.03f;
	float moveDeadZone = 0.10f;
	
	int misfireDamage = 1;
	float misfireShotSpeed = 0.1f;

	int sniperDamage = 1;
	float sniperChargeTime = 0.1f;
	float sniperBeamLifeSpan = 0.5f;
	glm::vec3 sniperBeamColour = { 0.5f,0.5f,1.0f };

	int overclockDamage = 1;
	float overclockChargeTime = 0.3f;
	float overclockBeamLifeSpan = 1.0f;
	glm::vec3 overclockBeamColour = { 1.0f,1.0f,0.5f };
	int enemyPierceCount = 99;
	int overclockReboundCount = 5;

	float eccoRefractionAngle = 30.0f;
	int eccoRefractionCount = 5;

	float chargedDuration = 0.0f;
	bool chargingShot = false;

	glm::vec2 fireDirection = {1.0f, 0.0f};

	float misfireColliderRadius = 0.1f;
	ModelRenderer* misfireModelRender = nullptr;
	std::string misfireModelPath = "SM_Sphere";
	std::string misfireMaterialPath;

};
