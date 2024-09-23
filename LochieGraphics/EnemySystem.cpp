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
#include "RenderSystem.h"
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
    if (!enemy)
    {
        meleeEnemyPoolCount = meleeInactivePool.size();
        return SpawnMelee(sceneObjects, pos);
    }
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

    if (!enemy)
    {
        rangedEnemyPoolCount = rangedInactivePool.size();
        return SpawnRanged(sceneObjects, pos);
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

    return hasBeenDespawned;
}

bool EnemySystem::DespawnRanged(SceneObject* sceneObject)
{
    bool hasBeenDespawned = false;
    
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
    for (auto& enemyPair : enemies)
    {
        glm::vec2 enemyPos2D = { transforms[enemyPair.first].getGlobalPosition().x, transforms[enemyPair.first].getGlobalPosition().z };
        int total = 0;
        int alignmentTotal = 0;
        glm::vec2 direction = {};
        float distanceToSync = FLT_MAX;

        std::vector<Hit> syncHits;
        std::vector<Hit> eccoHits;
        if (PhysicsSystem::RayCast(
            enemyPos2D, glm::normalize(syncPos2D - enemyPos2D), 
            syncHits, FLT_MAX,
            (int)CollisionLayers::sync | (int)CollisionLayers::base )
        ) {
            Hit hit = syncHits[0];
            if (hit.sceneObject->parts & Parts::sync)
            {
                distanceToSync = hit.distance;
                direction = glm::normalize(syncPos2D - enemyPos2D);
            }
        }
        if (PhysicsSystem::RayCast(
            enemyPos2D, glm::normalize(eccoPos2D - enemyPos2D),
            eccoHits, FLT_MAX,
            (int)CollisionLayers::ecco | (int)CollisionLayers::base )
            )
        {
            Hit hit = eccoHits[0];
            if (hit.sceneObject->parts & Parts::ecco)
            {
                if (hit.distance < distanceToSync)
                {
                    direction = glm::normalize(eccoPos2D - enemyPos2D);
                }
            }
        }

        if (glm::length(direction) > 0)
        {
            rigidbodies[enemyPair.first].vel = direction * maxSpeed;
        }
        else
        {
            glm::vec2 sep = {0.0f, 0.0f};
            glm::vec2 ali = {0.0f, 0.0f};
            glm::vec2 coh = {0.0f, 0.0f};

            for (auto& otherEnemyPair : enemies)
            {
                if (otherEnemyPair.first == enemyPair.first) continue;
                glm::vec2 otherEnemyPos2D = { transforms[otherEnemyPair.first].getGlobalPosition().x, transforms[otherEnemyPair.first].getGlobalPosition().z };
                float distance = glm::length(otherEnemyPos2D - enemyPos2D);
                if (distance <= perceptionRadius)
                {
                    glm::vec2 diff = enemyPos2D - otherEnemyPos2D;

                    sep += diff;
                    if (distance <= alignmentRadius)
                    {
                        ali += rigidbodies[otherEnemyPair.first].vel;
                        alignmentTotal++;
                    }

                    coh += otherEnemyPos2D;

                    total++;
                }
            }
            if (total == 0)
            {
                rigidbodies[enemyPair.first].vel = {0.0f, 0.0f};
            }
            else
            {
                sep *= seperationCoef;

                glm::vec2 sumAli = { 0.0f, 0.0f };
                if (alignmentTotal != 0)
                {
                    ali /= alignmentTotal;
                    sumAli = ali - rigidbodies[enemyPair.first].vel;
                    sumAli *= alignmentCoef;
                }

                coh /= total;
                glm::vec2 sumCoh = coh - enemyPos2D;
                sumCoh *= cohesionCoef;

                glm::vec2 vel = sep + sumAli + sumCoh;

                vel = Utilities::ClampMag(vel, 0, maxSpeed);

                RenderSystem::lines.DrawLineSegment(transforms[enemyPair.first].getGlobalPosition(), transforms[enemyPair.first].getGlobalPosition() + glm::vec3(vel.x*2, 0.0f, vel.y*2), {1,0,0});
                rigidbodies[enemyPair.first].vel = vel;
            }
        }
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
        AddUnlistedEnemiesToSystem(enemies, transforms);
    }
}

void EnemySystem::AddUnlistedEnemiesToSystem(
    std::unordered_map<unsigned long long, Enemy>& enemies,
    std::unordered_map<unsigned long long, Transform>& transforms
)
{
    for (auto& enemyPair : enemies)
    {
        bool isAlreadyMarked = false;
        switch (enemyPair.second.type)
        {
        case EnemyType::melee:

            for (int i = 0; i < meleeInactivePool.size(); i++)
            {
                if (meleeInactivePool[i] == enemyPair.first)
                {
                    isAlreadyMarked = true;
                    break;
                }
            }

            for (int i = 0; i < meleeActivePool.size(); i++)
            {
                if (meleeActivePool[i] == enemyPair.first)
                {
                    isAlreadyMarked = true;
                    break;
                }
            }
            if (isAlreadyMarked) break;

            else
            {
                Health* health = transforms[enemyPair.first].getSceneObject()->health();
                if (!health)
                {
                    health = new Health();
                    transforms[enemyPair.first].getSceneObject()->setHealth(health);
                }
                health->setMaxHealth(meleeEnemyHealth);
                health->currHealth = meleeEnemyHealth;
                health->onHealthZero.push_back([this](HealthPacket hp) { OnHealthZeroMelee(hp); });

                if(transforms[enemyPair.first].getGlobalPosition().y == 0.0f)
                    meleeActivePool.push_back(enemyPair.first);
                else
                {
                    meleeInactivePool.push_back(enemyPair.first);
                    meleeEnemyPoolCount++;
                }
            }

            break;

        case EnemyType::ranged:

            for (int i = 0; i < rangedInactivePool.size(); i++)
            {
                if (rangedInactivePool[i] == enemyPair.first)
                {
                    isAlreadyMarked = true;
                    break;
                }
            }

            for (int i = 0; i < rangedActivePool.size(); i++)
            {
                if (rangedActivePool[i] == enemyPair.first)
                {
                    isAlreadyMarked = true;
                    break;
                }
            }
            if (isAlreadyMarked) break;

            else
            {
                Health* health = transforms[enemyPair.first].getSceneObject()->health();
                if (!health)
                {
                    health = new Health();
                    transforms[enemyPair.first].getSceneObject()->setHealth(health);
                }
                health->setMaxHealth(rangedEnemyHealth);
                health->currHealth = rangedEnemyHealth;
                health->onHealthZero.push_back([this](HealthPacket hp) { OnHealthZeroRanged(hp); });


                if (transforms[enemyPair.first].getGlobalPosition().y == 0.0f)
                    rangedActivePool.push_back(enemyPair.first);
                else
                {
                    rangedInactivePool.push_back(enemyPair.first);
                    rangedEnemyPoolCount++;
                }
            }
            break;
        }
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

