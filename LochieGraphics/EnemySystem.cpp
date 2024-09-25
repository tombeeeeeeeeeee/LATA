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
#include "Collision.h"

#include "Utilities.h"

#include "Serialisation.h"
#include "EditorGUI.h"
#include "PhysicsSystem.h"
#include "RenderSystem.h"
#include "Hit.h"


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

void EnemySystem::SpawnExplosive(glm::vec3 pos)
{
    Scene* scene = SceneManager::scene;

    SceneObject* enemy = new SceneObject(scene, "ExplosiveEnemy" + std::to_string(explosiveEnemyCount++));
    enemy->setEnemy(new Enemy());
    enemy->setHealth(new Health());
    enemy->health()->setMaxHealth(explosiveEnemyHealth);
    enemy->health()->onHealthZero.push_back([this](HealthPacket hp) { OnHealthZeroExplosive(hp); });
    enemy->setRigidBody(new RigidBody());
    enemy->rigidbody()->invMass = 1.0f;
    enemy->setRenderer(
        explosiveEnemyRenderer
    );
    enemy->transform()->setParent(nullptr);
    enemy->transform()->setPosition(pos);
    enemy->health()->currHealth = explosiveEnemyHealth;
    enemy->rigidbody()->colliders = { new PolygonCollider({{0.0f,0.0f}}, explosiveEnemyColliderRadius, CollisionLayers::enemy) };
    enemy->rigidbody()->isStatic = false;
}

void EnemySystem::SpawnMelee(glm::vec3 pos)
{
    Scene* scene = SceneManager::scene;

    SceneObject* enemy = new SceneObject(scene, "MeleeEnemy" + std::to_string(meleeEnemyCount++));
    enemy->setEnemy(new Enemy());
    enemy->setHealth(new Health());
    enemy->health()->setMaxHealth(meleeEnemyHealth);
    enemy->health()->onHealthZero.push_back([this](HealthPacket hp) { OnHealthZeroMelee(hp); });
    enemy->setRigidBody(new RigidBody());
    enemy->rigidbody()->invMass = 1.0f;
    enemy->setRenderer(
        meleeEnemyRenderer
    );
    enemy->transform()->setParent(nullptr);
    enemy->transform()->setPosition(pos);
    enemy->health()->currHealth = meleeEnemyHealth;
    enemy->rigidbody()->colliders = { new PolygonCollider({{0.0f,0.0f}}, meleeEnemyColliderRadius, CollisionLayers::enemy) };
    enemy->rigidbody()->isStatic = false;
}

void EnemySystem::SpawnRanged(glm::vec3 pos)
{
    Scene* scene = SceneManager::scene;

    SceneObject* enemy = new SceneObject(scene, "RangedEnemy" + std::to_string(rangedEnemyCount++));
    enemy->setHealth(new Health());
    enemy->health()->setMaxHealth(rangedEnemyHealth);
    enemy->health()->currHealth = rangedEnemyHealth;
    enemy->health()->onHealthZero.push_back([this](HealthPacket hp) { OnHealthZeroRanged(hp); });
    enemy->setRigidBody(new RigidBody());
    enemy->rigidbody()->invMass = 1.0f;
    enemy->setRenderer(
        rangedEnemyRenderer
    );
    enemy->transform()->setParent(nullptr);
    enemy->transform()->setPosition(pos);
    enemy->rigidbody()->colliders = { new PolygonCollider({{0.0f,0.0f}}, meleeEnemyColliderRadius, CollisionLayers::enemy) };
    enemy->rigidbody()->isStatic = false;
}

bool EnemySystem::Despawn(SceneObject* sceneObject)
{

    if (sceneObject->parts & Parts::enemy)
    {
        SceneManager::scene->DeleteSceneObject(sceneObject->GUID);
        return true;
    }

    return false;
}


void EnemySystem::OnHealthZeroExplosive(HealthPacket healthpacket)
{
    if (healthpacket.so)
    {
        Despawn(healthpacket.so);
    }
}

void EnemySystem::OnHealthZeroMelee(HealthPacket healthpacket)
{
    if (healthpacket.so)
    {
        Despawn(healthpacket.so);
    }
}

void EnemySystem::OnHealthZeroRanged(HealthPacket healthpacket)
{
    if (healthpacket.so)
    {
        Despawn(healthpacket.so);
    }
}

void EnemySystem::TempBehaviour(
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

            
        glm::vec2 direction = {0.0f, 0.0f};
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
                enemyPair.second.lastTargetPos = syncPos2D;
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
                    enemyPair.second.lastTargetPos = eccoPos2D;
                }
            }
        }

        if (glm::length(direction) > 0)
        {
            rigidbodies[enemyPair.first].vel = direction * maxSpeed;
            enemyPair.second.hasLOS = false;
        }
        else 
        {
            enemyPair.second.hasLOS = true;
            float length = glm::length(enemyPair.second.lastTargetPos - enemyPos2D);
            if (length)
            {
                rigidbodies[enemyPair.first].vel = maxSpeed * (enemyPair.second.lastTargetPos - enemyPos2D) / length;
            }
            else
            {
                rigidbodies[enemyPair.first].vel = {0.0f, 0.0f};
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
        TempBehaviour(
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
}

void EnemySystem::SpawnEnemies(
    std::unordered_map<unsigned long long, Enemy>& enemies,
    std::unordered_map<unsigned long long, Transform>& transforms
)
{
    for (auto& enemyPair : enemies)
    {
        if (enemyPair.second.type & (int)EnemyType::spawnSpot)
        {
            if (enemyPair.second.type & (int)EnemyType::melee)
                SpawnMelee(transforms[enemyPair.first].getGlobalPosition());
            else if (enemyPair.second.type & (int)EnemyType::ranged)
                SpawnRanged(transforms[enemyPair.first].getGlobalPosition());
        }
    }
}

void EnemySystem::GUI()
{
    ImGui::Checkbox("AI Updating",&aiUpdating);

    ImGui::Text("AI STATS");
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
    };
}

void EnemySystem::OnMeleeCollision(Collision collision)
{
    if (collision.collisionMask & (int)CollisionLayers::sync || collision.collisionMask & (int)CollisionLayers::ecco)
    {
        collision.sceneObject->health()->subtractHealth(meleeEnemyDamage);
    }
}

