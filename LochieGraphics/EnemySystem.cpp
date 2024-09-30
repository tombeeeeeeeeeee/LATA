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
#include "Node.h"


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

void EnemySystem::LineOfSightAndTargetCheck(
    std::unordered_map<unsigned long long, Enemy>& enemies,
    std::unordered_map<unsigned long long, Transform>& transforms,
    std::unordered_map<unsigned long long, RigidBody>& rigidbodies,
    SceneObject* ecco, SceneObject* sync)
{

    glm::vec2 syncPos2D = { sync->transform()->getGlobalPosition().x, sync->transform()->getGlobalPosition().z };
    glm::vec2 eccoPos2D = { ecco->transform()->getGlobalPosition().x, ecco->transform()->getGlobalPosition().z };
    for (auto& enemyPair : enemies)
    {
        enemyPair.second.hasLOS = false;
        glm::vec2 enemyPos2D = { transforms[enemyPair.first].getGlobalPosition().x, transforms[enemyPair.first].getGlobalPosition().z };

        float distanceToSync = FLT_MAX;

        std::vector<Hit> syncHits;
        std::vector<Hit> eccoHits;
        if (PhysicsSystem::RayCast(
            enemyPos2D, glm::normalize(syncPos2D - enemyPos2D), 
            syncHits, FLT_MAX,
            (int)CollisionLayers::sync | (int)CollisionLayers::base )
        ) 
        {
            Hit hit = syncHits[0];
            if (hit.sceneObject->parts & Parts::sync)
            {
                distanceToSync = hit.distance;
                enemyPair.second.lastTargetPos = syncPos2D;
                enemyPair.second.hasLOS = true;
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
                    enemyPair.second.lastTargetPos = eccoPos2D;
                    enemyPair.second.hasLOS = true;
                }
            }
        }
    }
}

void EnemySystem::Steering(std::unordered_map<unsigned long long, Enemy>& enemies, std::unordered_map<unsigned long long, Transform>& transforms, std::unordered_map<unsigned long long, RigidBody>& rigidBodies, float delta)
{
    for (auto& enemyPair : enemies)
    {
        glm::vec2 avgPos = { 0.0f, 0.0f };
        glm::vec2 avgVel = { 0.0f, 0.0f };

        int totalNeighbours = 0;

        glm::vec2 enemyPos = transforms[enemyPair.first].get2DGlobalPosition();

        for (auto& otherEnemyPair : enemies)
        {
            glm::vec2 otherEnemyPos = transforms[otherEnemyPair.first].get2DGlobalPosition();
            if (enemyPair.first == otherEnemyPair.first) continue;
            glm::vec2 displacement = otherEnemyPos - enemyPos;
            float distanceSq = glm::dot(displacement, displacement);

            if (distanceSq <= perceptionRadius * perceptionRadius)
            {
                avgPos += otherEnemyPos;
                avgVel += otherEnemyPair.second.boidVelocity;

                totalNeighbours++;

                if (distanceSq <= separationRadius * separationRadius)
                {
                    glm::vec2 otherToSelf = enemyPos - otherEnemyPos;
                    float distance =  glm::length(otherToSelf);
                    float strength = 1.0f - (distance / separationRadius);
                
                    enemyPair.second.influenceThisFrame += (otherToSelf / distance) * strength * seperationCoef;
                }
            }
        }
        if (totalNeighbours == 0) continue;

        avgPos /= totalNeighbours;
        avgVel /= totalNeighbours;
        //Todo: enemyPair.second.lastTargetPos
        glm::vec2 alignmentForce = avgVel * alignmentCoef;
        glm::vec2 cohesionForce = (avgPos - enemyPos) * cohesionCoef;
        enemyPair.second.influenceThisFrame += alignmentForce + cohesionForce;
    }

    for (auto& enemyPair : enemies)
    {
        enemyPair.second.boidVelocity += enemyPair.second.influenceThisFrame;
        if (enemyPair.second.boidVelocity.x != 0.0f || enemyPair.second.boidVelocity.y != 0.0f)
        {
            enemyPair.second.boidVelocity = Utilities::ClampMag(enemyPair.second.boidVelocity, maxSpeed, maxSpeed);
        }
        enemyPair.second.influenceThisFrame = {0.0f, 0.0f};

        glm::vec2 curVel = rigidBodies[enemyPair.first].vel;
        glm::vec2 velocityDelta = enemyPair.second.boidVelocity - curVel;
        glm::vec2 forceThisFrame = velocityDelta / delta;
        rigidBodies[enemyPair.first].netForce += forceThisFrame;
    }

}

//TODO: Add AI Pathfinding and attacking in here.
void EnemySystem::Update   (
    std::unordered_map<unsigned long long, Enemy>& enemies,
    std::unordered_map<unsigned long long, Transform>& transforms, 
    std::unordered_map<unsigned long long, RigidBody>& rigidbodies, 
    SceneObject* ecco, SceneObject* sync, float delta
)
{
    if (aiUpdating)
    {
        LineOfSightAndTargetCheck(
            enemies,
            transforms,
            rigidbodies,
            ecco, sync
        );

        Steering(
            enemies, 
            transforms, 
            rigidbodies, 
            delta
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
    }   //
}

void EnemySystem::SpawnEnemiesInScene(
    std::unordered_map<unsigned long long, Enemy>& enemies,
    std::unordered_map<unsigned long long, Transform>& transforms,
    bool spawner
)
{
    for (auto& enemyPair : enemies)
    {
        if ((enemyPair.second.type & (int)EnemyType::spawnSpot) == (int)spawner)
        {
            if (enemyPair.second.type & (int)EnemyType::melee)
                SpawnMelee(transforms[enemyPair.first].getGlobalPosition());
            else if (enemyPair.second.type & (int)EnemyType::ranged)
                SpawnRanged(transforms[enemyPair.first].getGlobalPosition());
            else if (enemyPair.second.type & (int)EnemyType::explosive)
                SpawnExplosive(transforms[enemyPair.first].getGlobalPosition());
        }
    }
}

std::vector<Node*> EnemySystem::AStarSearch(Node* startNode, Node* endNode)
{
    std::vector<Node*> open;
    std::vector<Node*> closed;
    std::vector<Node*> path;

    Node* currNode = nullptr;
    int gScore = 0;

    if (startNode == nullptr || endNode == nullptr)
    {
        return std::vector<Node*>();
    }
    else if (startNode == endNode)
    {
        return std::vector<Node*>();
    }

    startNode->gScore = 0;
    startNode->previous = nullptr;

    open.push_back(startNode);

    do
    {
        //sort(open.begin(), open.end(), CompareNodes);
        currNode = open.back();
        open.pop_back();
        if (currNode == endNode)  break;
        closed.push_back(currNode);

        for (auto con : currNode->connections)
        {
            if (find(closed.begin(), closed.end(), con.target) == closed.end())
            {
                gScore = currNode->gScore + con.cost;
                float hScore = Hueristic(con.target, endNode);
                float fScore = gScore + hScore;
                if (find(open.begin(), open.end(), con.target) == open.end())
                {
                    con.target->gScore = gScore;
                    con.target->fScore = fScore;
                    con.target->previous = currNode;
                    open.push_back(con.target);
                }
                else if (fScore < con.target->fScore)
                {
                    con.target->gScore = gScore;
                    con.target->fScore = fScore;
                    con.target->previous = currNode;
                }
            }
        }

    } while (open.size() != 0);

    if (open.empty()) return std::vector<Node*>();

    currNode = endNode;
    do
    {
        path.insert(path.begin(), currNode);
        currNode = currNode->previous;
    } while (currNode != nullptr);

    return path;
}

float EnemySystem::Hueristic(Node* pos, Node* end)
{
    return 0.0f;
}

bool EnemySystem::CompareNodes(Node* a, Node* b)
{
    return a->fScore > b->fScore;
}

void EnemySystem::GUI()
{
    ImGui::Checkbox("AI Updating",&aiUpdating);

    ImGui::Text("AI STATS");
    ImGui::DragFloat("Max Speed", &maxSpeed);
    ImGui::DragFloat("Alignment Coef", &alignmentCoef);
    ImGui::DragFloat("Cohesion Coef", &cohesionCoef);
    ImGui::DragFloat("Seperation Coef", &seperationCoef);

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

    ImGui::Text("BOID TESTING");
    ImGui::DragFloat("Perception Radius", &perceptionRadius);
    ImGui::DragFloat("Separation Radius", &separationRadius);

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

