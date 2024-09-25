#pragma once

#include "Maths.h"

#include <vector>
#include <string>
#include <unordered_map>

struct Enemy;
class Transform;
class RigidBody;
class SceneObject;
class ModelRenderer;
struct HealthPacket;
struct Collision;

namespace toml {
	inline namespace v3 {
		class table;
	}
}

//Might need these later.
//class Ecco;
//class Sync;

class EnemySystem
{
private:
	float maxSpeed = 700.0f;

	int explosiveEnemyCount = 0;
	int meleeEnemyCount = 0;
	int rangedEnemyCount = 0;

public:
	bool aiUpdating = false;

	EnemySystem() {};
	EnemySystem(toml::table table);
	void Start();


	int explosiveEnemyHealth = 0;
	float explosiveEnemyMoveSpeed = 0;
	int explosiveEnemyDamage = 0;
	float explosiveEnemyColliderRadius = 120.0f;
	ModelRenderer* explosiveEnemyRenderer = nullptr;
	std::string explosiveEnemyModel = "SM_Sphere";
	std::string explosiveEnemyMaterialPath = "images/otherskybox/nx.png";


	int meleeEnemyHealth = 0;
	float meleeEnemyMoveSpeed = 0;
	int meleeEnemyDamage = 0;
	float meleeEnemyColliderRadius = 120.0f;
	ModelRenderer* meleeEnemyRenderer = nullptr;
	std::string meleeEnemyModel = "SM_Sphere";
	std::string meleeEnemyMaterialPath = "images/otherskybox/nx.png";

	int rangedEnemyHealth = 0;
	float rangedEnemyMoveSpeed = 0;
	int rangedEnemyDamage = 0;
	float rangedEnemyColliderRadius = 120.0f;
	ModelRenderer* rangedEnemyRenderer = nullptr;
	std::string rangedEnemyModel = "SM_Sphere";
	std::string rangedEnemyMaterialPath = "images/otherskybox/nx.png";

	void SpawnExplosive(glm::vec3 pos);
	void SpawnMelee(glm::vec3 pos);
	void SpawnRanged(glm::vec3 pos);
	bool Despawn(SceneObject* sceneObject);

	void OnHealthZeroExplosive(HealthPacket healthPacket);
	void OnHealthZeroMelee(HealthPacket healthPacket);
	void OnHealthZeroRanged(HealthPacket healthPacket);

	void TempBehaviour(
		std::unordered_map<unsigned long long, Enemy>& enemies,
		std::unordered_map<unsigned long long, Transform>& transforms,
		std::unordered_map<unsigned long long, RigidBody>& rigidbodies,
		SceneObject* ecco, SceneObject* sync
	);

	//TODO: ADD AI PATHFINDING, and general AI STUFF IN HERE
	void Update(
		std::unordered_map<unsigned long long, Enemy>& enemies,
		std::unordered_map<unsigned long long, Transform>& transforms,
		std::unordered_map<unsigned long long, RigidBody>& rigidBodies,
		SceneObject* ecco, SceneObject* sync,
		float delta
	);

	void SpawnEnemies(
		std::unordered_map<unsigned long long, Enemy>& enemies,
		std::unordered_map<unsigned long long, Transform>& transforms
	);

	void GUI();

	toml::table Serialise() const;

	void OnMeleeCollision(Collision collision);
};

