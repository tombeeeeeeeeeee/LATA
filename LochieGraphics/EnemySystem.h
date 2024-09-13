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
class ModelRenderer;

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

	EnemySystem();
	EnemySystem(toml::table table);
	void Start();

	int meleeEnemyHealth = 0;
	float meleeEnemyMoveSpeed = 0;
	int meleeEnemyDamage = 0;
	float meleeEnemyColliderRadius = 0.0f;
	ModelRenderer* meleeEnemyRenderer;
	std::string meleeEnemyModel = "models/Sphere.fbx";
	std::string meleeEnemyMaterialPath = "images/otherskybox/nx.png";

	int rangedEnemyHealth = 0;
	float rangedEnemyMoveSpeed = 0;
	int rangedEnemyDamage = 0;
	float rangedEnemyColliderRadius = 0.0f;
	ModelRenderer* rangedEnemyRenderer;
	std::string rangedEnemyModel = "models/Sphere.fbx";
	std::string rangedEnemyMaterialPath = "images/otherskybox/nx.png";

	glm::vec3 offscreenSpawnPosition = {-1000.0f,-3000.0f,-1000.0f};

	unsigned long long SpawnMelee(std::unordered_map<unsigned long long, SceneObject*>& sceneObjects, glm::vec3 pos);
	unsigned long long SpawnRanged(std::unordered_map<unsigned long long, SceneObject*>& sceneObjects, glm::vec3 pos);
	bool DespawnMelee(std::unordered_map<unsigned long long, SceneObject*>& sceneObjects, unsigned long long GUID);
	bool DespawnRanged(std::unordered_map<unsigned long long, SceneObject*>& sceneObjects, unsigned long long GUID);

	//TODO: ADD AI PATHFINDING, and general AI STUFF IN HERE
	void Update(
		std::unordered_map<unsigned long long, Enemy>& enemies,
		std::unordered_map<unsigned long long, Transform>& transforms,
		SceneObject* ecco, SceneObject* sync,
		float delta
	);

	toml::table Serialise() const;

	std::vector<unsigned long long> InitialiseMelee(std::unordered_map<unsigned long long, SceneObject*>& sceneObjects, int count);
	std::vector<unsigned long long> InitialiseRanged(std::unordered_map<unsigned long long, SceneObject*>& sceneObjects, int count);
};

