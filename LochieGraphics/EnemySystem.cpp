#include "EnemySystem.h"
#include "Enemy.h"
#include "Transform.h"
#include "RigidBody.h"
#include "SceneObject.h"
#include "Health.h"
#include "SceneManager.h"

unsigned long long EnemySystem::SpawnMelee(std::unordered_map<unsigned long long, SceneObject>& sceneObjects, glm::vec3 pos)
{
    unsigned long long GUID = 0;

    //If there is no enemies left in the pool make a new enemy
    if (meleeEnemyPoolCount <= 0)
    {
        GUID = InitialiseMelee(1)[0];
    }
    else
    {
        GUID = meleeInactivePool[--meleeEnemyPoolCount];
        meleeInactivePool.pop_back();
    }

    SceneObject* enemy = &sceneObjects[GUID];
    meleeActivePool.push_back(GUID);

    enemy->transform()->setPosition(pos);
    enemy->health()->currHealth = meleeEnemyHealth;
    enemy->rigidbody()->colliders = { &meleeEnemyCollider };
    enemy->rigidbody()->isStatic = false;

    return GUID;
}

unsigned long long EnemySystem::SpawnRanged(std::unordered_map<unsigned long long, SceneObject>& sceneObjects, glm::vec3 pos)
{
    unsigned long long GUID = 0;

    if (rangedEnemyPoolCount <= 0)
    {
        GUID = InitialiseRanged(1)[0];
    }
    else
    {
        GUID = rangedInactivePool[--rangedEnemyPoolCount];
        rangedInactivePool.pop_back();
    }

    SceneObject* enemy = &sceneObjects[GUID];
    rangedActivePool.push_back(GUID);

    enemy->transform()->setPosition(pos);
    enemy->health()->currHealth = rangedEnemyHealth;
    enemy->rigidbody()->colliders = { &rangedEnemyCollider };
    enemy->rigidbody()->isStatic = false;

    return GUID;
}

bool EnemySystem::DespawnMelee(std::unordered_map<unsigned long long, SceneObject>* sceneObjects, unsigned long long GUID)
{
    bool hasBeenDespawned = false;

    for (auto i = meleeActivePool.begin(); i != meleeActivePool.end(); i++)
    {
        if ((*i) == GUID)
        {
            hasBeenDespawned = true;
            meleeActivePool.erase(i);
            meleeInactivePool.push_back(GUID);
            SceneObject* enemy = &(*sceneObjects)[GUID];
            enemy->transform()->setPosition(offscreenSpawnPosition);
            enemy->rigidbody()->colliders = {};
            enemy->rigidbody()->isStatic = true;
            break;
        }
    }
    return hasBeenDespawned;
}

bool EnemySystem::DespawnRanged(std::unordered_map<unsigned long long, SceneObject>* sceneObjects, unsigned long long GUID)
{
    bool hasBeenDespawned = false;

    for (auto i = rangedActivePool.begin(); i != rangedActivePool.end(); i++)
    {
        if ((*i) == GUID)
        {
            hasBeenDespawned = true;
            rangedActivePool.erase(i);
            rangedInactivePool.push_back(GUID);
            SceneObject* enemy = &(*sceneObjects)[GUID];
            enemy->transform()->setPosition(offscreenSpawnPosition);
            enemy->rigidbody()->colliders = {};
            enemy->rigidbody()->isStatic = true;
            break;
        }
    }
    return hasBeenDespawned;
}

//TODO: Add AI Pathfinding and attacking in here.
void EnemySystem::Update(
    std::unordered_map<unsigned long long, Enemy>& enemies,
    std::unordered_map<unsigned long long, Transform>& transforms, 
    SceneObject* ecco, SceneObject* sync, float delta
)
{
}

std::vector<unsigned long long> EnemySystem::InitialiseMelee(std::unordered_map<unsigned long long, SceneObject>& sceneObjects, int count)
{
    std::vector<unsigned long long> enemiesSpawned;
    enemiesSpawned.reserve(count);
    Scene* scene = SceneManager::scene;

    for (int i = 0; i < count; i++)
    {
        SceneObject enemy = SceneObject(scene, "MeleeEnemy" + i );
        enemy.setHealth(new Health());
        enemy.health()->setMaxHealth(meleeEnemyHealth);
        enemy.setRigidBody(new RigidBody());
        enemy.rigidbody()->invMass = 1.0f;
        //TODO: Add meshRender
        scene->sceneObjects[enemy.GUID] = enemy;
        enemiesSpawned.push_back(enemy.GUID);
    }
    meleeInactivePool.insert(meleeInactivePool.end(), enemiesSpawned.begin(), enemiesSpawned.end());

    return enemiesSpawned;
}

std::vector<unsigned long long> EnemySystem::InitialiseRanged(std::unordered_map<unsigned long long, SceneObject>& sceneObjects, int count)
{
    std::vector<unsigned long long> enemiesSpawned;
    enemiesSpawned.reserve(count);
    Scene* scene = SceneManager::scene;

    for (int i = 0; i < count; i++)
    {
        SceneObject enemy = SceneObject(scene, "MeleeEnemy" + i);
        enemy.setHealth(new Health());
        enemy.health()->setMaxHealth(rangedEnemyHealth);
        enemy.setRigidBody(new RigidBody());
        enemy.rigidbody()->invMass = 1.0f;
        //TODO: Add meshRender
        scene->sceneObjects[enemy.GUID] = enemy;
        enemiesSpawned.push_back(enemy.GUID);
    }
    rangedInactivePool.insert(rangedInactivePool.end(), enemiesSpawned.begin(), enemiesSpawned.end());

    return enemiesSpawned;
}

