#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include "Collider.h"
#include "Maths.h"

struct Enemy;
class Transform;
class RigidBody;
class SceneObject;

//Might need these later.
//class Ecco;
//class Sync;

class EnemySystem
{
private:
	int meleeEnemyPoolCount = 0;
	std::vector<unsigned long long> meleeInactivePool;
	std::vector<unsigned long long> meleeActivePool;

	int rangedEnemyPoolCount = 0;
	std::vector<unsigned long long> rangedInactivePool;
	std::vector<unsigned long long> rangedActivePool;

public:
	int meleeEnemyHealth = 0.0f;
	int meleeEnemyMoveSpeed = 0.0f;
	int meleeEnemyDamage = 0.0f;
	Collider meleeEnemyCollider;
	std::string meleeEnemyModel;
	std::string meleeEnemyMaterial;

	int rangedEnemyHealth = 0.0f;
	int rangedEnemyMoveSpeed = 0.0f;
	int rangedEnemyDamage = 0.0f;
	Collider rangedEnemyCollider;
	std::string rangedEnemyModel;
	std::string rangedEnemyMaterial;

	glm::vec3 offscreenSpawnPosition = {-100.0f,-300.0f,-100.0f};

	unsigned long long SpawnMelee(std::unordered_map<unsigned long long, SceneObject>& sceneObjects, glm::vec3 pos);
	unsigned long long SpawnRanged(std::unordered_map<unsigned long long, SceneObject>& sceneObjects, glm::vec3 pos);
	bool DespawnMelee(std::unordered_map<unsigned long long, SceneObject>* sceneObjects, unsigned long long GUID);
	bool DespawnRanged(std::unordered_map<unsigned long long, SceneObject>* sceneObjects, unsigned long long GUID);

	//TODO: ADD AI PATHFINDING, and general AI STUFF IN HERE
	void Update(
		std::unordered_map<unsigned long long, Enemy>& enemies,
		std::unordered_map<unsigned long long, Transform>& transforms,
		SceneObject* ecco, SceneObject* sync,
		float delta
	);

	std::vector<unsigned long long> InitialiseMelee(std::unordered_map<unsigned long long, SceneObject>& sceneObjects, int count);
	std::vector<unsigned long long> InitialiseRanged(std::unordered_map<unsigned long long, SceneObject>& sceneObjects, int count);
};

