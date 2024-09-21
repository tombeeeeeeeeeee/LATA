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
#include "Collider.h"
#include "ModelRenderer.h"
#include "Scene.h"
#include "Paths.h"

#include "Utilities.h"

#include "Serialisation.h"
#include "EditorGUI.h"
#include "PhysicsSystem.h"
#include "Hit.h"

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
    inactiveMeleeParent = new SceneObject(SceneManager::scene, "Inactive Melee Enemies");
    inactiveRangedParent = new SceneObject(SceneManager::scene, "Inactive Ranged Enemies");

    if (!meleeEnemyRenderer)
    {
        Material* meleeEnemyMaterial = ResourceManager::defaultMaterial;

        meleeEnemyRenderer = new ModelRenderer(
            ResourceManager::LoadModelAsset(Paths::modelSaveLocation + meleeEnemyModel + Paths::modelExtension),
            meleeEnemyMaterial
        );
    }

    if (!rangedEnemyRenderer)
    {
        Material* rangedEnemyMaterial = ResourceManager::defaultMaterial;

        rangedEnemyRenderer = new ModelRenderer(
            ResourceManager::LoadModelAsset(Paths::modelSaveLocation + rangedEnemyModel + Paths::modelExtension),
            rangedEnemyMaterial
        );
    }
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

    enemy->transform()->setParent(nullptr);
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
    enemy->transform()->setParent(nullptr);
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
            enemy->transform()->setParent(inactiveMeleeParent->transform());
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
            enemy->transform()->setParent(inactiveRangedParent->transform());
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

void EnemySystem::Boiding(
    std::unordered_map<unsigned long long, Enemy>& enemies,
    std::unordered_map<unsigned long long, Transform>& transforms,
    std::unordered_map<unsigned long long, RigidBody>& rigidbodies,
    SceneObject* ecco, SceneObject* sync)
{

    glm::vec2 syncPos2D = { sync->transform()->getGlobalPosition().x, sync->transform()->getGlobalPosition().z };
    glm::vec2 eccoPos2D = { ecco->transform()->getGlobalPosition().x, ecco->transform()->getGlobalPosition().z };
    for (auto& enemyGUID : meleeActivePool)
    {
        glm::vec2 enemyPos2D = { transforms[enemyGUID].getGlobalPosition().x, transforms[enemyGUID].getGlobalPosition().z };
        int total = 0;
        glm::vec2 direction = {};
        float distanceToSync = FLT_MAX;

        std::vector<Hit> syncHits;
        std::vector<Hit> eccoHits;
        if (PhysicsSystem::RayCast(
            enemyPos2D, glm::normalize(syncPos2D - enemyPos2D), 
            syncHits, perceptionRadius, 
            (int)CollisionLayers::sync | (int)CollisionLayers::base)
        )
        {
            for (Hit& hit : syncHits)
            {
                if (hit.sceneObject->parts & Parts::sync)
                {
                    distanceToSync = hit.distance;
                    direction = glm::normalize(syncPos2D - enemyPos2D);
                    total = 1;
                }
            }
        }
        if (PhysicsSystem::RayCast(
            enemyPos2D, glm::normalize(eccoPos2D - enemyPos2D),
            eccoHits, perceptionRadius,
            (int)CollisionLayers::ecco | (int)CollisionLayers::base)
            )
        {
            for (Hit& hit : eccoHits)
            {
                if (hit.sceneObject->parts & Parts::ecco)
                {
                    if (hit.distance < distanceToSync)
                    {
                        direction = glm::normalize(eccoPos2D - enemyPos2D);
                        total = 1;
                    }
                }
            }
        }


        glm::vec2 sep;
        glm::vec2 ali = direction;
        glm::vec2 coh = direction;

        for (auto& otherEnemyGUID : meleeActivePool)
        {
            if (otherEnemyGUID == enemyGUID) continue;
            glm::vec2 otherEnemyPos2D = { transforms[otherEnemyGUID].getGlobalPosition().x,transforms[otherEnemyGUID].getGlobalPosition().z };
            float distance = glm::distance(otherEnemyPos2D, enemyPos2D);
            if (distance <= perceptionRadius)
            {
                glm::vec2 diff = enemyPos2D - otherEnemyPos2D;
                
                diff /= distance * distance;
                sep += diff;

                ali += rigidbodies[otherEnemyGUID].vel;

                coh += otherEnemyPos2D;

                total++;
            }
        }
        if (total == 0) continue;

        sep /= total;
        sep *= maxSpeed;
        glm::vec2 sumSep = sep - rigidbodies[enemyGUID].vel;
        sumSep = Utilities::ClampMag(sumSep, 0, maxForce);
        sumSep *= seperationCoef;

        ali /= total;
        ali *= maxSpeed;
        glm::vec2 sumAli = ali - rigidbodies[enemyGUID].vel;
        sumAli = Utilities::ClampMag(sumAli, 0, maxForce);
        sumAli *= alignmentCoef;

        coh /= total;
        glm::vec2 sumCoh = coh - enemyPos2D;
        sumCoh = Utilities::ClampMag(sumCoh, 0, maxSpeed);
        sumCoh = sumCoh - rigidbodies[enemyGUID].vel;
        sumCoh = Utilities::ClampMag(sumCoh, 0, maxForce);
        sumCoh *= cohesionCoef;

        glm::vec2 force = sumSep + sumAli + sumCoh;
        rigidbodies[enemyGUID].netForce += force;
    }
}

//TODO: Add AI Pathfinding and attacking in here.
void EnemySystem::Update(
    std::unordered_map<unsigned long long, Enemy>& enemies,
    std::unordered_map<unsigned long long, Transform>& transforms, 
    std::unordered_map<unsigned long long, RigidBody>& rigidbodies, 
    SceneObject* ecco, SceneObject* sync, float delta
)
{
    if (aiUpdating)
    {
        Boiding(
            enemies,
            transforms,
            rigidbodies,
            ecco, sync
        );

        //TODO ADD States and Transitions
        //for (auto& enemyPair : enemies)
        //{
        //    Enemy& enemy = enemyPair.second;
        //    SceneObject* agent = SceneManager::scene->sceneObjects[enemyPair.first];
        //    State* newState = nullptr;
        //
        //    // check the current state's transitions
        //    for (auto& t : enemy.state->transitions)
        //    {
        //        if (t.condition->IsTrue(agent))
        //        {
        //            newState = t.targetState;
        //            break;
        //        }
        //    }
        //
        //
        //    if (newState != nullptr && newState != enemy.state)
        //    {
        //        enemy.state->Exit(agent);
        //        enemy.state = newState;
        //        enemy.state->Enter(agent);
        //    }
        //
        //    enemy.state->Update(agent);
        //}
    }
    if (addEnemiesThisUpdate)
    {

    }
    
}

void EnemySystem::GUI()
{
    ImGui::Checkbox("AI Updating",&aiUpdating);

    if(ImGui::Button("Add New Enemies to list"))
    {
        addEnemiesThisUpdate = true;
    }

    ImGui::Text("AI STATS");
    ImGui::DragFloat("Sense Radius", &perceptionRadius);
    ImGui::DragFloat("Alignment Coefficient", &alignmentCoef);
    ImGui::DragFloat("Cohesion Coefficient", &cohesionCoef);
    ImGui::DragFloat("Seperation Coefficient", &seperationCoef);
    ImGui::DragFloat("Max Force", &maxForce);
    ImGui::DragFloat("Max Speed", &maxSpeed);




    ImGui::Text("MELEE ENEMY STATS");
    ImGui::DragInt("Melee Enemy Health", &meleeEnemyHealth);
    ImGui::DragFloat("Melee Enemy Move Speed", &meleeEnemyMoveSpeed);
    ImGui::DragInt("Melee Enemy Damage", &meleeEnemyDamage);
    ImGui::DragFloat("Melee Enemy Collider Radius", &meleeEnemyColliderRadius);
    if (ResourceManager::ModelAssetSelector("Melee Enemy Model", &(meleeEnemyRenderer->model)))
    {
        meleeEnemyModel = meleeEnemyRenderer->model->path;
    }
    ImGui::InputText("Melee Enemy Material", &meleeEnemyMaterialPath);
    ImGui::Text("");
    ImGui::Text("RANGED ENEMY STATS");
    ImGui::DragInt("Ranged Enemy Health", &rangedEnemyHealth);
    ImGui::DragFloat("Ranged Enemy Move Speed", &rangedEnemyMoveSpeed);
    ImGui::DragInt("Ranged Enemy Damage", &rangedEnemyDamage);
    ImGui::DragFloat("Ranged Enemy Collider Radius", &rangedEnemyColliderRadius);
    if (ResourceManager::ModelAssetSelector("Ranged Enemy Model", &(rangedEnemyRenderer->model)))
    {
        rangedEnemyModel = rangedEnemyRenderer->model->path;
    }
    ImGui::InputText("Ranged Enemy Material", &rangedEnemyMaterialPath);

    ImGui::End();
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
        enemy->transform()->setParent(inactiveMeleeParent->transform());
        enemy->transform()->setPosition(offscreenSpawnPosition);
        enemy->setEnemy(new Enemy());
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
        enemy->transform()->setParent(inactiveRangedParent->transform());
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

