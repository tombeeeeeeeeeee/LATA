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
	float perceptionRadius = 3000.0f;
	float alignmentRadius = 3000.0f;
	float alignmentCoef = 0.5f;
	float cohesionCoef = 0.5f;
	float seperationCoef = 0.5f;

	float maxSpeed = 700.0f;

	int meleeEnemyCount = 0;
	int rangedEnemyCount = 0;

	bool aiUpdating = false;
	bool addEnemiesThisUpdate = false;

public:

	EnemySystem();
	EnemySystem(toml::table table);
	void Start();

	int meleeEnemyHealth = 0;
	float meleeEnemyMoveSpeed = 0;
	int meleeEnemyDamage = 0;
	float meleeEnemyColliderRadius = 30.0f;
	ModelRenderer* meleeEnemyRenderer;
	std::string meleeEnemyModel = "SM_Sphere";
	std::string meleeEnemyMaterialPath = "images/otherskybox/nx.png";

	int rangedEnemyHealth = 0;
	float rangedEnemyMoveSpeed = 0;
	int rangedEnemyDamage = 0;
	float rangedEnemyColliderRadius = 30.0f;
	ModelRenderer* rangedEnemyRenderer;
	std::string rangedEnemyModel = "SM_Sphere";
	std::string rangedEnemyMaterialPath = "images/otherskybox/nx.png";

	glm::vec3 offscreenSpawnPosition = {-1000.0f,-3000.0f,-1000.0f};

	unsigned long long SpawnMelee(std::unordered_map<unsigned long long, SceneObject*>& sceneObjects, glm::vec3 pos);
	unsigned long long SpawnRanged(std::unordered_map<unsigned long long, SceneObject*>& sceneObjects, glm::vec3 pos);
	bool DespawnMelee(SceneObject* sceneObject);
	bool DespawnRanged(SceneObject* sceneObject);

	void OnHealthZeroMelee(HealthPacket health);
	void OnHealthZeroRanged(HealthPacket health);

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

	//void OnMeleeCollision(Collision collision);
};

