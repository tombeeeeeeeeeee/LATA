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
class SpawnManager;
class Health;
struct HealthPacket;
struct Collision;
struct Collider;
struct Node;
struct Edge;
struct PolygonCollider;

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
	float maxSpeed = 200.0f;

	int explosiveEnemyCount = 0;
	int meleeEnemyCount = 0;
	int rangedEnemyCount = 0;
	float timeForEnemiesToSpawnAgain = 60.0f;
public:
	bool aiUpdating = false;
	const int nfmDensity = 20;
	const int maxNormalInfluence = 250;

	EnemySystem() {};
	EnemySystem(toml::table table);
	void Start(
		std::unordered_map<unsigned long long, Transform>& transforms,
		std::unordered_map<unsigned long long, RigidBody>& rigidbodies,
		std::unordered_map<unsigned long long, Collider*>& colliders
	);

	float perceptionRadius = 500.0f;
	float separationRadius = 300.0f;

	float alignmentCoef = 0.5f;
	float cohesionCoef = 0.5f;
	float seperationCoef = 0.5f;
	float normalCoef = 0.5f;
	float playerCoef = 0.5f;

	float slowedPercentage = 75.0f;

	int explosiveEnemyHealth = 2;
	float explosiveEnemyColliderRadius = 120.0f;
	float timeToExplode = 2.0f;
	float explosionRadius = 350.0f;
	int explosionDamage = 2;
	float distanceToExplode = 150.0f;
	ModelRenderer* explosiveEnemyRenderer = nullptr;
	std::string explosiveEnemyModel = "SM_Enemy_TPose";
	std::string explosiveEnemyMaterialPath = "images/otherskybox/nx.png";

	int meleeEnemyHealth = 3;
	int meleeEnemyDamage = 0;
	float meleeEnemyColliderRadius = 120.0f;
	float timeToPunch = 0.8f;
	float distanceToPunch = 150.0f;
	float punchRadius = 150.0f;
	ModelRenderer* meleeEnemyRenderer = nullptr;
	std::string meleeEnemyModel = "SM_Enemy_TPose";
	std::string meleeEnemyMaterialPath = "images/otherskybox/nx.png";

	int rangedEnemyHealth = 3;
	int rangedEnemyDamage = 0;
	float rangedEnemyColliderRadius = 120.0f;
	float timeToShoot = 0.5f;
	float distanceToShoot = 500.0f;
	float distanceToFlee = 400.0f;
	ModelRenderer* rangedEnemyRenderer = nullptr;
	std::string rangedEnemyModel = "SM_Sphere";
	std::string rangedEnemyMaterialPath = "images/otherskybox/nx.png";

	void SpawnExplosive(glm::vec3 pos);
	void SpawnMelee(glm::vec3 pos);
	void SpawnRanged(glm::vec3 pos);

	void Update(
		std::unordered_map<unsigned long long, Enemy>& enemies,
		std::unordered_map<unsigned long long, Transform>& transforms,
		std::unordered_map<unsigned long long, RigidBody>& rigidBodies,
		std::unordered_map<unsigned long long, Health>& healths,
		std::unordered_map<unsigned long long, SpawnManager>& spawnManagers,
		glm::vec2 eccoPos, glm::vec2 syncPos,
		float delta
	);

	void SpawnEnemiesInScene(
		std::unordered_map<unsigned long long, Enemy>& enemies,
		std::unordered_map<unsigned long long, Transform>& transforms,
		bool spawner = true
	);

	void GUI();

	toml::table Serialise() const;


	glm::ivec2 mapMinCorner = { 0.0f, 0.0f };
	glm::ivec2 mapDimensions = { 0.0f, 0.0f };

	std::vector<glm::vec2> normalFlowMap;

	void UpdateNormalFlowMap(
		std::unordered_map<unsigned long long, Transform>& transforms,
		std::unordered_map<unsigned long long, RigidBody>& rigidBodies,
		std::unordered_map<unsigned long long, Collider*>& colliders
	);
	void LoadNormalFlowMapFromImage(unsigned char* image, int width, int height);
	void PopulateNormalFlowMap(
		std::unordered_map<unsigned long long, Transform>& transforms,
		std::unordered_map<unsigned long long, RigidBody>& rigidbodies,
		std::unordered_map<unsigned long long, Collider*>& colliders
	);

	void PopulateNormalFlowMapFromCircle(
		PolygonCollider* circle,
		Transform& transform,
		std::vector<glm::vec3>& normalFlowMapVec3
	);
	void PopulateNormalFlowMapFromPoly(
		PolygonCollider* poly,
		Transform& transform, 
		std::vector<glm::vec3>& normalFlowMapVec3
	);


private:

	void LineOfSightAndTargetCheck(
		Enemy enemy, Transform transform, RigidBody rigidBody,
		SceneObject* ecco, SceneObject* sync
	);

	void AbilityCheck(
		std::unordered_map<unsigned long long, Enemy>& enemies,
		std::unordered_map<unsigned long long, RigidBody>& rigidBodies,
		std::unordered_map<unsigned long long, Transform>& transforms,
		glm::vec2 eccoPos, glm::vec2 syncPos,
		float delta
	);

	void Steering(
		std::unordered_map<unsigned long long, Enemy>& enemies,
		std::unordered_map<unsigned long long, Transform>& transforms,
		std::unordered_map<unsigned long long, RigidBody>& rigidbodies,
		glm::vec2 eccoPos, glm::vec2 syncPos,
		float delta
		);
	
	void HealthCheck(
		std::unordered_map<unsigned long long, Enemy>& enemies,
		std::unordered_map<unsigned long long, Health>& healths
	);

	void UpdateSpawnManagers(
		std::unordered_map<unsigned long long, Transform>& transform,
		std::unordered_map<unsigned long long, SpawnManager>& spawnManagers,
		float delta
	);

	glm::vec2 GetNormalFlowInfluence(glm::vec2 pos);

};

