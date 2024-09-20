#include "EnemySystem.h"
#include "Enemy.h"
#include "State.h"
#include "Condition.h"
#include "Transform.h"
#include "RigidBody.h"
#include "SceneObject.h"
#include "Health.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "Paths.h"

#include "Serialisation.h"

EnemySystem::EnemySystem()
{
}

EnemySystem::EnemySystem(toml::table table)
{
    meleeEnemyHealth = Serialisation::LoadAsInt(table["meleeEnemyHealth"]);
    meleeEnemyMoveSpeed = Serialisation::LoadAsFloat(table["meleeEnemyMoveSpeed"]);
    meleeEnemyDamage = Serialisation::LoadAsInt(table["meleeEnemyDamage"]);
    meleeEnemyColliderRadius = Serialisation::LoadAsFloat(table["meleeEnemyColliderRadius"]);
    meleeEnemyModel = Serialisation::LoadAsString(table["meleeEnemyModel"]);
    meleeEnemyMaterialPath = Serialisation::LoadAsString(table["meleeEnemyMaterialPath"]);

    rangedEnemyHealth = Serialisation::LoadAsInt(table["rangedEnemyHealth"]);
    rangedEnemyMoveSpeed = Serialisation::LoadAsFloat(table["rangedEnemyMoveSpeed"]);
    rangedEnemyDamage = Serialisation::LoadAsInt(table["rangedEnemyDamage"]);
    rangedEnemyColliderRadius = Serialisation::LoadAsFloat(table["rangedEnemyColliderRadius"]);
    rangedEnemyModel = Serialisation::LoadAsString(table["rangedEnemyModel"]);
    rangedEnemyMaterialPath = Serialisation::LoadAsString(table["rangedEnemyMaterialPath"]);

    glm::vec3 offscreenSpawnPosition = Serialisation::LoadAsVec3(table["offscreenSpawnPosition"]);
}

void EnemySystem::Start()
{
    Material* meleeEnemyMaterial = ResourceManager::defaultMaterial;

    meleeEnemyRenderer = new ModelRenderer(
        ResourceManager::LoadModelAsset(Paths::modelSaveLocation + meleeEnemyModel + Paths::modelExtension),
        meleeEnemyMaterial
    );

    Material* rangedEnemyMaterial = ResourceManager::defaultMaterial;

    rangedEnemyRenderer = new ModelRenderer(
        ResourceManager::LoadModelAsset(Paths::modelSaveLocation + rangedEnemyModel + Paths::modelExtension),
        rangedEnemyMaterial
    );


}

unsigned long long EnemySystem::SpawnMelee(std::unordered_map<unsigned long long, SceneObject*>& sceneObjects, glm::vec3 pos)
{
    unsigned long long GUID = 0;

    //If there is no enemies left in the pool make a new enemy
    if (meleeEnemyPoolCount <= 0)
    {
        GUID = InitialiseMelee(sceneObjects, 1)[0];
    }
    else
    {
        GUID = meleeInactivePool[--meleeEnemyPoolCount];
        meleeInactivePool.pop_back();
    }

    SceneObject* enemy = sceneObjects[GUID];
    meleeActivePool.push_back(GUID);

    enemy->transform()->setPosition(pos);
    enemy->health()->currHealth = meleeEnemyHealth;
    enemy->rigidbody()->colliders = { new PolygonCollider({{0.0f,0.0f}}, meleeEnemyColliderRadius, CollisionLayers::enemy) };
    enemy->rigidbody()->isStatic = false;

    return GUID;
}

unsigned long long EnemySystem::SpawnRanged(std::unordered_map<unsigned long long, SceneObject*>& sceneObjects, glm::vec3 pos)
{
    unsigned long long GUID = 0;

    if (rangedEnemyPoolCount <= 0)
    {
        GUID = InitialiseRanged(sceneObjects, 1)[0];
    }
    else
    {
        GUID = rangedInactivePool[--rangedEnemyPoolCount];
        rangedInactivePool.pop_back();
    }

    SceneObject* enemy = sceneObjects[GUID];
    rangedActivePool.push_back(GUID);

    enemy->transform()->setPosition(pos);
    enemy->health()->currHealth = rangedEnemyHealth;
    enemy->rigidbody()->colliders = { new PolygonCollider({{0.0f,0.0f}}, meleeEnemyColliderRadius, CollisionLayers::enemy) };
    enemy->rigidbody()->isStatic = false;

    return GUID;
}

bool EnemySystem::DespawnMelee(SceneObject* sceneObject)
{
    bool hasBeenDespawned = false;
    int GUID = sceneObject->GUID;

    for (auto i = meleeActivePool.begin(); i != meleeActivePool.end(); i++)
    {
        if ((*i) == GUID)
        {
            hasBeenDespawned = true;
            meleeActivePool.erase(i);
            meleeInactivePool.push_back(GUID);
            SceneObject* enemy = sceneObject;
            enemy->transform()->setPosition(offscreenSpawnPosition);
            enemy->rigidbody()->colliders = {};
            enemy->rigidbody()->isStatic = true;
            break;
        }
    }
    return hasBeenDespawned;
}

bool EnemySystem::DespawnRanged(SceneObject* sceneObject)
{
    bool hasBeenDespawned = false;
    int GUID = sceneObject->GUID;

    for (auto i = meleeActivePool.begin(); i != meleeActivePool.end(); i++)
    {
        if ((*i) == GUID)
        {
            hasBeenDespawned = true;
            rangedActivePool.erase(i);
            rangedInactivePool.push_back(GUID);
            SceneObject* enemy = sceneObject;
            enemy->transform()->setPosition(offscreenSpawnPosition);
            enemy->rigidbody()->colliders = {};
            enemy->rigidbody()->isStatic = true;
            break;
        }
    }
    return hasBeenDespawned;
}

void EnemySystem::OnHealthZeroMelee(HealthPacket healthpacket)
{
    if (healthpacket.so)
    {
        DespawnMelee(healthpacket.so);
    }
}

void EnemySystem::OnHealthZeroRanged(HealthPacket healthpacket)
{
    if (healthpacket.so)
    {
        DespawnRanged(healthpacket.so);
    }
}

//TODO: Add AI Pathfinding and attacking in here.
void EnemySystem::Update(
    std::unordered_map<unsigned long long, Enemy>& enemies,
    std::unordered_map<unsigned long long, Transform>& transforms, 
    SceneObject* ecco, SceneObject* sync, float delta
)
{
    for (auto& enemyPair : enemies)
    {
        Enemy& enemy = enemyPair.second;
        SceneObject* agent = SceneManager::scene->sceneObjects[enemyPair.first];
        State* newState = nullptr;

        // check the current state's transitions
        for (auto& t : enemy.state->transitions)
        {
            if (t.condition->IsTrue(agent)) 
            {
                newState = t.targetState;
                break;
            }
        }


        if (newState != nullptr && newState != enemy.state)
        {
            enemy.state->Exit(agent);
            enemy.state = newState;
            enemy.state->Enter(agent);
        }

        enemy.state->Update(agent);
    }
}

toml::table EnemySystem::Serialise() const
{
    return toml::table{
        { "meleeEnemyHealth", meleeEnemyHealth },
        { "meleeEnemyMoveSpeed", meleeEnemyMoveSpeed },
        { "meleeEnemyDamage", meleeEnemyDamage },
        { "meleeEnemyColliderRadius", meleeEnemyColliderRadius },
        { "meleeEnemyModel", meleeEnemyModel },
        { "meleeEnemyMaterialPath", meleeEnemyMaterialPath },
        { "rangedEnemyHealth", rangedEnemyHealth },
        { "rangedEnemyMoveSpeed", rangedEnemyMoveSpeed },
        { "rangedEnemyDamage", rangedEnemyDamage }, 
        { "rangedEnemyColliderRadius", rangedEnemyColliderRadius },
        { "rangedEnemyModel", rangedEnemyModel },
        { "rangedEnemyMaterialPath", rangedEnemyMaterialPath },
        { "offscreenSpawnPosition", Serialisation::SaveAsVec3(offscreenSpawnPosition) },
    };
}

std::vector<unsigned long long> EnemySystem::InitialiseMelee(std::unordered_map<unsigned long long, SceneObject*>& sceneObjects, int count)
{
    std::vector<unsigned long long> enemiesSpawned;
    enemiesSpawned.reserve(count);
    Scene* scene = SceneManager::scene;

    for (int i = 0; i < count; i++)
    {
        SceneObject* enemy = new SceneObject(scene, "MeleeEnemy" + std::to_string(meleeEnemyPoolCount));
        enemy->transform()->setPosition(offscreenSpawnPosition);
        enemy->setHealth(new Health());
        enemy->health()->setMaxHealth(meleeEnemyHealth);
        enemy->health()->onHealthZero.push_back([this](HealthPacket hp) { OnHealthZeroMelee(hp); });
        enemy->setRigidBody(new RigidBody());
        enemy->rigidbody()->invMass = 1.0f;
        enemy->setRenderer(
            meleeEnemyRenderer
        );
        enemiesSpawned.push_back(enemy->GUID);
        meleeEnemyPoolCount++;
    }
    meleeInactivePool.insert(meleeInactivePool.end(), enemiesSpawned.begin(), enemiesSpawned.end());

    return enemiesSpawned;
}

std::vector<unsigned long long> EnemySystem::InitialiseRanged(std::unordered_map<unsigned long long, SceneObject*>& sceneObjects, int count)
{
    std::vector<unsigned long long> enemiesSpawned;
    enemiesSpawned.reserve(count);
    Scene* scene = SceneManager::scene;

    for (int i = 0; i < count; i++)
    {
        SceneObject* enemy = new SceneObject(scene, "RangedEnemy" + std::to_string(rangedEnemyPoolCount));
        enemy->transform()->setPosition(offscreenSpawnPosition);
        enemy->setHealth(new Health());
        enemy->health()->setMaxHealth(rangedEnemyHealth);
        enemy->health()->onHealthZero.push_back([this](HealthPacket hp) { OnHealthZeroRanged(hp); });
        enemy->setRigidBody(new RigidBody());
        enemy->rigidbody()->invMass = 1.0f;
        enemy->setRenderer(
            rangedEnemyRenderer
        );
        enemiesSpawned.push_back(enemy->GUID);
        rangedEnemyPoolCount++;
    }
    rangedInactivePool.insert(rangedInactivePool.end(), enemiesSpawned.begin(), enemiesSpawned.end());

    return enemiesSpawned;
}

