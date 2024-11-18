#include "EnemySystem.h"

#include "Enemy.h"
#include "Transform.h"
#include "RigidBody.h"
#include "SceneObject.h"
#include "Health.h"
#include "SceneManager.h"
#include "SpawnManager.h"
#include "ResourceManager.h"
#include "Collider.h"
#include "ModelRenderer.h"
#include "Scene.h"
#include "Collision.h"

#include "Utilities.h"

#include "Serialisation.h"
#include "EditorGUI.h"
#include "PhysicsSystem.h"
#include "RenderSystem.h"
#include "Hit.h"
#include "Paths.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include <iostream>
#include <filesystem>
#include <ostream>
#include <fstream>


void EnemySystem::Load(toml::table table)
{
    maxSpeed = Serialisation::LoadAsFloat(table["maxSpeed"]);
    alignmentCoef = Serialisation::LoadAsFloat(table["alignmentCoef"]);
    cohesionCoef = Serialisation::LoadAsFloat(table["cohesionCoef"]);
    seperationCoef = Serialisation::LoadAsFloat(table["seperationCoef"]);
    normalCoef = Serialisation::LoadAsFloat(table["normalCoef"]);
    playerCoef = Serialisation::LoadAsFloat(table["playerCoef"]);
    slowedPercentage = Serialisation::LoadAsFloat(table["slowedPercentage"]);
    perceptionRadius = Serialisation::LoadAsFloat(table["perceptionRadius"]);
    separationRadius = Serialisation::LoadAsFloat(table["separationRadius"]);
    timeForEnemiesToSpawnAgain = Serialisation::LoadAsFloat(table["timeForEnemiesToSpawnAgain"]);

    explosiveEnemyHealth = Serialisation::LoadAsInt(table["explosiveEnemyHealth"]);
    timeToExplode = Serialisation::LoadAsFloat(table["timeToExplode"]);
    speedWhileExploding = Serialisation::LoadAsFloat(table["speedWhileExploding"]);
    explosionDamage = Serialisation::LoadAsInt(table["explosionDamage"]);
    explosionRadius = Serialisation::LoadAsFloat(table["explosionRadius"]);
    distanceToExplode = Serialisation::LoadAsFloat(table["distanceToExplode"]);
    explosiveEnemyColliderRadius = Serialisation::LoadAsFloat(table["explosiveEnemyColliderRadius"]);

    meleeEnemyHealth = Serialisation::LoadAsInt(table["meleeEnemyHealth"]);
    meleeEnemyDamage = Serialisation::LoadAsInt(table["meleeEnemyDamage"]);
    meleeEnemyColliderRadius = Serialisation::LoadAsFloat(table["meleeEnemyColliderRadius"]);
    distanceToPunch = Serialisation::LoadAsFloat(table["distanceToPunch"]);
    timeToPunch = Serialisation::LoadAsFloat(table["timeToPunch"]);
    meleeEnemyModel = Serialisation::LoadAsString(table["meleeEnemyModel"]);
    meleeEnemyMaterialPath = Serialisation::LoadAsString(table["meleeEnemyMaterialPath"]);

    rangedEnemyHealth = Serialisation::LoadAsInt(table["rangedEnemyHealth"]);
    rangedEnemyDamage = Serialisation::LoadAsInt(table["rangedEnemyDamage"]);
    rangedEnemyColliderRadius = Serialisation::LoadAsFloat(table["rangedEnemyColliderRadius"]);
    distanceToShoot = Serialisation::LoadAsFloat(table["distanceToShoot"]);
    distanceToFlee = Serialisation::LoadAsFloat(table["distanceToFlee"]);
    rangedEnemyModel = Serialisation::LoadAsString(table["rangedEnemyModel"]);
    rangedEnemyMaterialPath = Serialisation::LoadAsString(table["rangedEnemyMaterialPath"]);
}

void EnemySystem::Start(
    std::unordered_map<unsigned long long, Transform>& transforms,
    std::unordered_map<unsigned long long, RigidBody>& rigidbodies,
    std::unordered_map<unsigned long long, Collider*>& colliders
)
{
    if (!explosiveEnemyRenderer)
    {
        Material* explosiveEnemyMaterial = ResourceManager::LoadMaterialAsset(Paths::materialSaveLocation + "Enemy 2" + Paths::materialExtension);

        explosiveEnemyRenderer = new ModelRenderer(
            ResourceManager::LoadModelAsset(Paths::modelSaveLocation + explosiveEnemyModel + Paths::modelExtension),
            explosiveEnemyMaterial
        );
    }

    if (!meleeEnemyRenderer)
    {
        Material* meleeEnemyMaterial = ResourceManager::LoadMaterialAsset(Paths::materialSaveLocation + "Enemy 1" + Paths::materialExtension);

        meleeEnemyRenderer = new ModelRenderer(
            ResourceManager::LoadModelAsset(Paths::modelSaveLocation + meleeEnemyModel + Paths::modelExtension),
            meleeEnemyMaterial
        );
    }

    if (!rangedEnemyRenderer)
    {
        Material* rangedEnemyMaterial = ResourceManager::LoadMaterialAsset(Paths::materialSaveLocation + "Enemy 2" + Paths::materialExtension);

        rangedEnemyRenderer = new ModelRenderer(
            ResourceManager::LoadModelAsset(Paths::modelSaveLocation + rangedEnemyModel + Paths::modelExtension),
            rangedEnemyMaterial
        );
    }

    explosiveEnemyIdle = ResourceManager::LoadAnimation(explosiveEnemyAnimationPath, explosiveEnemyRenderer->model);
    meleeEnemyIdle = ResourceManager::LoadAnimation(meleeEnemyAnimationPath, meleeEnemyRenderer->model);
    rangedEnemyIdle = ResourceManager::LoadAnimation(rangedEnemyAnimationPath, rangedEnemyRenderer->model);

    UpdateNormalFlowMap(transforms, rigidbodies, colliders);
}

void EnemySystem::SpawnExplosive(glm::vec3 pos, std::string tag)
{
    Scene* scene = SceneManager::scene;

    SceneObject* enemy = new SceneObject(scene, "ExplosiveEnemy" + std::to_string(explosiveEnemyCount++));
    enemy->setEnemy(new Enemy((int)EnemyType::explosive, tag, getEnemyFrameIndex()));
    enemy->setHealth(new Health());
    enemy->health()->setMaxHealth(explosiveEnemyHealth);
    enemy->setRigidBody(new RigidBody());
    enemy->rigidbody()->invMass = 1.0f;
    enemy->setRenderer(
        explosiveEnemyRenderer
    );
    enemy->setAnimator(new Animator(explosiveEnemyIdle));
    enemy->transform()->setParent(nullptr);
    enemy->transform()->setPosition(pos);
    enemy->health()->currHealth = explosiveEnemyHealth;
    enemy->rigidbody()->addCollider(new PolygonCollider({{0.0f,0.0f}}, explosiveEnemyColliderRadius, CollisionLayers::enemy));
    enemy->rigidbody()->isStatic = false;
}

void EnemySystem::SpawnMelee(glm::vec3 pos, std::string tag)
{
    Scene* scene = SceneManager::scene;

    SceneObject* enemy = new SceneObject(scene, "MeleeEnemy" + std::to_string(meleeEnemyCount++));
    enemy->setEnemy(new Enemy((int)EnemyType::melee, tag, getEnemyFrameIndex()));
    enemy->setHealth(new Health());
    enemy->health()->setMaxHealth(meleeEnemyHealth);
    enemy->setRigidBody(new RigidBody());
    enemy->rigidbody()->invMass = 1.0f;
    enemy->setRenderer(
        meleeEnemyRenderer
    );
    enemy->setAnimator(new Animator(meleeEnemyIdle));
    enemy->transform()->setParent(nullptr);
    enemy->transform()->setPosition(pos);
    enemy->health()->currHealth = meleeEnemyHealth;
    enemy->rigidbody()->addCollider(new PolygonCollider({{0.0f,0.0f}}, meleeEnemyColliderRadius, CollisionLayers::enemy) );
    enemy->rigidbody()->isStatic = false;
}

void EnemySystem::SpawnRanged(glm::vec3 pos, std::string tag)
{
    Scene* scene = SceneManager::scene;

    SceneObject* enemy = new SceneObject(scene, "RangedEnemy" + std::to_string(rangedEnemyCount++));
    enemy->setEnemy(new Enemy((int)EnemyType::ranged, tag, getEnemyFrameIndex()));
    enemy->setHealth(new Health());
    enemy->health()->setMaxHealth(rangedEnemyHealth);
    enemy->health()->currHealth = rangedEnemyHealth;
    enemy->setRigidBody(new RigidBody());
    enemy->rigidbody()->invMass = 1.0f;
    enemy->setRenderer(
        rangedEnemyRenderer
    );
    enemy->setAnimator(new Animator(rangedEnemyIdle));
    enemy->transform()->setParent(nullptr);
    enemy->transform()->setPosition(pos);
    enemy->rigidbody()->addCollider(new PolygonCollider({{0.0f,0.0f}}, meleeEnemyColliderRadius, CollisionLayers::enemy) );
    enemy->rigidbody()->isStatic = false;
}



void EnemySystem::LineOfSightAndTargetCheck(
    std::unordered_map<unsigned long long, Enemy>& enemies,
    std::unordered_map<unsigned long long, RigidBody>& rigidBodies,
    std::unordered_map<unsigned long long, Transform>& transforms,
    glm::vec2 eccoPos, glm::vec2 syncPos
)
{
    for (auto& pair : enemies)
    {
        if (frameCount != pair.second.frameForCheck) continue;
        glm::vec2 pos = transforms[pair.first].get2DGlobalPosition();
        float distanceToSync = glm::length(syncPos - pos); 
        float distanceToEcco = glm::length(eccoPos - pos);

        std::vector<Hit> eccoHits;
        std::vector<Hit> syncHits;

        bool eccoWasHit = !PhysicsSystem::RayCast(pos, glm::normalize(eccoPos - pos), eccoHits, distanceToEcco, ((int)CollisionLayers::base | (int)CollisionLayers::softCover));
        bool syncWasHit = !PhysicsSystem::RayCast(pos, glm::normalize(syncPos - pos), syncHits, distanceToSync, ((int)CollisionLayers::base | (int)CollisionLayers::softCover));

        
        if (!eccoWasHit && !syncWasHit)
        {
            pair.second.hasLOS = false;
        }
        else
        {
            pair.second.hasLOS = true;
            pair.second.hasTarget = true;
            if (distanceToEcco < distanceToSync)
            {
                pair.second.target = eccoPos;
            }
            else
            {
                pair.second.target = syncPos;
            }
        }
    }
}

void EnemySystem::AbilityCheck(
    std::unordered_map<unsigned long long, Enemy>& enemies,
    std::unordered_map<unsigned long long, RigidBody>& rigidBodies,
    std::unordered_map<unsigned long long, Transform>& transforms,
    glm::vec2 eccoPos, glm::vec2 syncPos,
    float delta
)
{

    for (auto& enemyPair : enemies)
    {
        glm::vec2 enemyPos = transforms[enemyPair.first].get2DGlobalPosition();
        glm::vec2 eccoDelta = eccoPos - enemyPos;
        glm::vec2 syncDelta = syncPos - enemyPos;

        if (enemyPair.second.inAbility)
        {
            int type = enemyPair.second.type;
            std::vector<Hit> hits;
            switch (type)
            {
            case (int)EnemyType::explosive:
                if (enemyPair.second.timeInAbility < timeToExplode)
                {
                    enemyPair.second.timeInAbility += delta;
                }
                else
                {
                    enemyPair.second.inAbility = false;

                    hits = PhysicsSystem::CircleCast(enemyPos, explosionRadius, (int)CollisionLayers::sync | (int)CollisionLayers::ecco);
                    for (auto& hit : hits)
                    {
                        Health* health = hit.sceneObject->health();
                        if (health)
                        {
                            health->subtractHealth(explosionDamage);
                        }
                    }
                    SceneManager::scene->audio.PlaySound(Audio::enemyExplode);
                    SceneManager::scene->DeleteSceneObjectAndChildren(enemyPair.first);
                }
                break;
            case (int)EnemyType::melee:
                if (enemyPair.second.timeInAbility < timeToPunch)
                {
                    enemyPair.second.timeInAbility += delta;
                }
                else
                {
                    enemyPair.second.inAbility = false;
                    glm::vec3 f = transforms[enemyPair.first].left();

                    hits = PhysicsSystem::CircleCast(enemyPos + glm::vec2(f.x * meleeEnemyColliderRadius, f.z * meleeEnemyColliderRadius), punchRadius, (int)CollisionLayers::sync | (int)CollisionLayers::ecco);
                    for (auto& hit : hits)
                    {
                        Health* health = hit.sceneObject->health();
                        if (health)
                        {
                            health->subtractHealth(meleeEnemyDamage);
                        }
                    }
                }
                break;
            case (int)EnemyType::ranged:
                if (enemyPair.second.timeInAbility < timeToPunch)
                {
                    enemyPair.second.timeInAbility += delta;
                }
                else
                {
                }
                break;
            }
        }
        else
        {
            int type = enemyPair.second.type;
            switch (type)
            {
            case (int)EnemyType::explosive:
                if (glm::dot(eccoDelta, eccoDelta) < distanceToExplode * distanceToExplode || glm::dot(syncDelta, syncDelta) < distanceToExplode * distanceToExplode)
                {
                    enemyPair.second.inAbility = true;
                }
                break;
            case (int)EnemyType::melee:
                if (glm::dot(eccoDelta, eccoDelta) < distanceToPunch * distanceToPunch || glm::dot(syncDelta, syncDelta) < distanceToPunch * distanceToPunch)
                {
                    enemyPair.second.inAbility = true;
                    enemyPair.second.timeInAbility = 0.0f;
                }
                break;
            case (int)EnemyType::ranged:
                if (glm::dot(eccoDelta, eccoDelta) < distanceToFlee * distanceToFlee || glm::dot(syncDelta, syncDelta) < distanceToFlee * distanceToFlee)
                {
                    enemyPair.second.fleeing = true;
                } 
                else if (glm::dot(eccoDelta, eccoDelta) < distanceToShoot * distanceToShoot || glm::dot(syncDelta, syncDelta) < distanceToShoot * distanceToShoot)
                {
                    enemyPair.second.inAbility = true;
                    enemyPair.second.fleeing = false;
                }
                else enemyPair.second.fleeing = false;
                break;
            }
        }
    }
}

void EnemySystem::Steering(
    std::unordered_map<unsigned long long, Enemy>& enemies,
    std::unordered_map<unsigned long long, Transform>& transforms,
    std::unordered_map<unsigned long long, RigidBody>& rigidBodies,
    glm::vec2 eccoPos, glm::vec2 syncPos,
    float delta
)
{
    for (auto& enemyPair : enemies)
    {

        if (enemyPair.second.inAbility)
        {
            if (enemyPair.second.type & (int)EnemyType::explosive)
            {
                glm::vec2 playerForce = glm::normalize(enemyPair.second.target - transforms[enemyPair.first].get2DGlobalPosition());
                playerForce *= 100 * playerCoef;
                enemyPair.second.influenceThisFrame += playerForce;
                if(drawForceLines) RenderSystem::lines.DrawLineSegement2D(transforms[enemyPair.first].get2DGlobalPosition(), transforms[enemyPair.first].get2DGlobalPosition() + playerForce * 10.0f, { 1,1,0 }, 100);
            }
            else
            {
                float length = glm::length(rigidBodies[enemyPair.first].vel);
                if (length == 0.0f) continue;
                float slowedLength = length * slowedPercentage / 100;
                enemyPair.second.influenceThisFrame = -glm::normalize(rigidBodies[enemyPair.first].vel) * fminf(length, slowedLength);
            }
        }
        else
        {
            if (enemyPair.second.type & (int)EnemyType::spawnSpot) continue;
            glm::vec2 avgPos = { 0.0f, 0.0f };
            glm::vec2 avgVel = { 0.0f, 0.0f };

            int totalNeighbours = 0;

            glm::vec2 enemyPos = transforms[enemyPair.first].get2DGlobalPosition();

            for (auto& otherEnemyPair : enemies)
            {
                if (otherEnemyPair.second.type & (int)EnemyType::spawnSpot) continue;
                if (otherEnemyPair.second.inAbility) continue;
                if (otherEnemyPair.second.fleeing != enemyPair.second.fleeing) continue;

                glm::vec2 otherEnemyPos = transforms[otherEnemyPair.first].get2DGlobalPosition();
                if (enemyPair.first == otherEnemyPair.first) continue;
                glm::vec2 displacement = otherEnemyPos - enemyPos;
                float distanceSq = glm::dot(displacement, displacement);

                if (distanceSq <= perceptionRadius * perceptionRadius)
                {
                    avgPos += otherEnemyPos;
                    avgVel += otherEnemyPair.second.boidVelocity;

                    totalNeighbours++;

                    if (distanceSq <= separationRadius * separationRadius && distanceSq != 0)
                    {
                        glm::vec2 otherToSelf = enemyPos - otherEnemyPos;
                        float distance = glm::length(otherToSelf);
                        float strength = 1.0f - (distance / separationRadius);

                        enemyPair.second.influenceThisFrame += (otherToSelf / distance) * strength * seperationCoef;
                    }
                }
            }
            if (drawForceLines)
            RenderSystem::lines.DrawLineSegement2D(enemyPos, enemyPos + enemyPair.second.influenceThisFrame * 10.0f, { 0,0,1 }, 100);
            float enemyLOSNormalMultiplier = 100.0f;
            if (enemyPair.second.hasLOS)
            {
                enemyLOSNormalMultiplier = 1.0f;
                glm::vec2 tooTarget = glm::normalize(enemyPair.second.target - transforms[enemyPair.first].get2DGlobalPosition());
                if (glm::dot(tooTarget, tooTarget) > 0)
                {
                    glm::vec2 playerForce = glm::normalize(enemyPair.second.target - transforms[enemyPair.first].get2DGlobalPosition());
                    playerForce *= 100 * playerCoef * (enemyPair.second.fleeing ? -1 : 1);
                    enemyPair.second.influenceThisFrame += playerForce;
                    if (drawForceLines)
                    {
                        RenderSystem::lines.DrawLineSegement2D(enemyPos, enemyPos + playerForce * 10.0f, { 1,1,0 }, 100);
                    }
                }

            }
            else if(enemyPair.second.hasTarget)
            {
                if (glm::length(enemyPair.second.target - transforms[enemyPair.first].get2DGlobalPosition()) > 20.0f)
                {
                    glm::vec2 tooTarget = glm::normalize(enemyPair.second.target - transforms[enemyPair.first].get2DGlobalPosition());
                    if (glm::dot(tooTarget, tooTarget) > 0)
                    {
                        glm::vec2 playerForce = glm::normalize(enemyPair.second.target - transforms[enemyPair.first].get2DGlobalPosition());
                        playerForce *= 100 * playerCoef * (enemyPair.second.fleeing ? -1 : 1);
                        enemyPair.second.influenceThisFrame += playerForce;
                        if (drawForceLines)
                        {
                            RenderSystem::lines.DrawLineSegement2D(enemyPos, enemyPos + playerForce * 10.0f, { 1,1,0 }, 100);
                        }
                    }
                }
                else
                {
                    enemyPair.second.hasTarget = false;
                }
            }

            glm::vec2 normalForce = GetNormalFlowInfluence(enemyPos) * normalCoef * enemyLOSNormalMultiplier;
            enemyPair.second.influenceThisFrame += normalForce;

            if (drawForceLines)
            {
                RenderSystem::lines.DrawLineSegement2D(enemyPos, enemyPos + normalForce * 10.0f, { 1,0,0 }, 100);
            }
            if (totalNeighbours == 0) continue;

            avgPos /= totalNeighbours;
            avgVel /= totalNeighbours;
            //Todo: enemyPair.second.lastTargetPos

            glm::vec2 alignmentForce = avgVel * alignmentCoef;
            glm::vec2 cohesionForce = (avgPos - enemyPos) * cohesionCoef;

            if (drawForceLines)
            {
                RenderSystem::lines.DrawLineSegement2D(enemyPos, enemyPos + cohesionForce * 10.0f,  { 0,1,0 }, 100);
                RenderSystem::lines.DrawLineSegement2D(enemyPos, enemyPos + alignmentForce * 10.0f, { 0,1,1 }, 100);
            }

            enemyPair.second.influenceThisFrame += alignmentForce + cohesionForce;

            if (isnan(enemyPair.second.influenceThisFrame.x))
            {
                std::cout << "Error Here" << std::endl;
            }
        }
    }

    for (auto& enemyPair : enemies)
    {
        enemyPair.second.boidVelocity += enemyPair.second.influenceThisFrame;
        if (enemyPair.second.boidVelocity.x != 0.0f || enemyPair.second.boidVelocity.y != 0.0f)
        {
            if (enemyPair.second.inAbility && enemyPair.second.type == (int)EnemyType::explosive)
                enemyPair.second.boidVelocity = Utilities::ClampMag(enemyPair.second.boidVelocity, speedWhileExploding, speedWhileExploding);
            else
                enemyPair.second.boidVelocity = Utilities::ClampMag(enemyPair.second.boidVelocity, maxSpeed, maxSpeed);
        }
        enemyPair.second.influenceThisFrame = {0.0f, 0.0f};


        glm::vec2 curVel = rigidBodies[enemyPair.first].vel;
        glm::vec2 velocityDelta = enemyPair.second.boidVelocity - curVel;
        glm::vec2 forceThisFrame = velocityDelta / delta;
        rigidBodies[enemyPair.first].netForce += forceThisFrame;
        enemyPair.second.aim = Utilities::Lerp(enemyPair.second.aim, enemyPair.second.boidVelocity, 0.1f);

        float angle = atan2f(enemyPair.second.aim.x, enemyPair.second.aim.y) * 180.0f / PI;
        if (isnan(angle)) continue;
        glm::vec3 eulers = transforms[enemyPair.first].getEulerRotation();
        eulers.y = angle;
        transforms[enemyPair.first].setEulerRotation(eulers);
    }

}

    void EnemySystem::HealthCheck(std::unordered_map<unsigned long long, Enemy>& enemies, std::unordered_map<unsigned long long, Health>& healths)
    {
        for (auto& enemyPair : enemies)
        {
            if (enemyPair.second.type & (int)EnemyType::spawnSpot) continue;

            auto health = healths.find(enemyPair.first);
            if (health != healths.end() && healths[enemyPair.first].currHealth <= 0)
            {
                enemyTags.at(enemyPair.second.tag)--;
                SceneManager::scene->DeleteSceneObjectAndChildren(enemyPair.first);
            }
        }
    }

    void EnemySystem::UpdateSpawnManagers(std::unordered_map<unsigned long long, Transform>& transforms, std::unordered_map<unsigned long long, SpawnManager>& spawnManagers, float delta)
    {
        for (auto& spawnPair : spawnManagers)
        {
            SpawnManager* spawner = &spawnPair.second;
            if (spawner->spawning || spawner->triggeredOnce) spawner->timeSinceLastSpawn += delta;

            if (spawnPair.second.spawning)
            {
                if (spawnPair.second.spawnPattern.size() == 0) continue;
                if (!spawner->triggeredOnce)
                {
                    int count = 1;
                    for (auto& child : transforms[spawnPair.first].getChildren()) count++;
                    AddTag(spawner->enemyTriggerTag, count * spawner->numToSpawn);
                }

                if (spawner->timeSinceLastSpawn >= spawner->timeBetweenSpawns)
                {
                    spawner->timeSinceLastSpawn = 0;
                    spawner->currSpawnCount++;
                    spawner->triggeredOnce = true;
                    spawner->spawning = spawner->currSpawnCount < spawner->numToSpawn;
                    int enemyType = spawner->spawnPattern[spawner->indexInSpawnLoop];
                    spawner->indexInSpawnLoop = (spawner->indexInSpawnLoop + 1) % spawner->spawnPattern.size();
                    
                    switch (enemyType)
                    {
                    case 0:
                        SpawnExplosive(transforms[spawnPair.first].getGlobalPosition(), spawner->enemyTriggerTag);
                        break;
                    case 1:
                        SpawnMelee(transforms[spawnPair.first].getGlobalPosition(), spawner->enemyTriggerTag);
                        break;
                    case 2:
                        SpawnRanged(transforms[spawnPair.first].getGlobalPosition(), spawner->enemyTriggerTag);
                        break;
                    }
                    for (auto& child : transforms[spawnPair.first].getChildren())
                    {
                        switch (enemyType)
                        {
                        case 0:
                            SpawnExplosive(child->getGlobalPosition(), spawner->enemyTriggerTag);
                            break;
                        case 1:
                            SpawnMelee(child->getGlobalPosition(), spawner->enemyTriggerTag);
                            break;
                        case 2:
                            SpawnRanged(child->getGlobalPosition(), spawner->enemyTriggerTag);
                            break;
                        }
                    }
                }

                if (spawner->triggeredOnce && !spawner->spawning)
                {
                    spawner->spawning = spawner->timeSinceLastSpawn >= timeForEnemiesToSpawnAgain;
                }
            }
        }
    }

glm::vec2 EnemySystem::GetNormalFlowInfluence(glm::vec2 pos)
{
    if (isnan(pos.x) || isnan(pos.y)) return { 0.0f, 0.0f };
    if (pos.x < mapMinCorner.x) return { 0.0f, 0.0f };
    if (pos.y < mapMinCorner.y) return { 0.0f, 0.0f };

    glm::vec2 actualDimensions = mapDimensions * nfmDensity;
    if (pos.x > mapMinCorner.x + actualDimensions.x) return { 0.0f, 0.0f };
    if (pos.y > mapMinCorner.y + actualDimensions.y) return { 0.0f, 0.0f };
    pos -= mapMinCorner;
    pos /= nfmDensity;

    glm::vec2 normalInfluence = normalFlowMap[(int)pos.x + (int)mapDimensions.x * (int)(mapDimensions.y - pos.y - 1)];
    return { normalInfluence.x, normalInfluence.y };
}

int EnemySystem::getEnemyFrameIndex()
{
    int returnValue = enemyFrameIndex;
    enemyFrameIndex = (enemyFrameIndex + 1) % frameCountMin;
    return returnValue;
}


//TODO: Add AI Pathfinding and attacking in here.
void EnemySystem::Update   (
    std::unordered_map<unsigned long long, Enemy>& enemies,
    std::unordered_map<unsigned long long, Transform>& transforms, 
    std::unordered_map<unsigned long long, RigidBody>& rigidbodies,
    std::unordered_map<unsigned long long, Health>& healths,
    std::unordered_map<unsigned long long, SpawnManager>& spawnManagers,
    glm::vec2 eccoPos, glm::vec2 syncPos, float delta
)
{
    frameCount = (frameCount + 1) % frameCountMin;

    if (aiUpdating)
    {
        AbilityCheck(
            enemies,
            rigidbodies,
            transforms,
            eccoPos, syncPos, delta
        );

        LineOfSightAndTargetCheck(
            enemies,
            rigidbodies,
            transforms,
            eccoPos, syncPos
        );

        Steering(
            enemies, 
            transforms, 
            rigidbodies, 
            eccoPos, syncPos,
            delta
        );
        
        HealthCheck(enemies, healths);
        
        for (auto& triggerTag : enemyTags)
        {
            int enemiesWithTagRemaining = triggerTag.second;
            TriggerSystem::TriggerTag(triggerTag.first, enemiesWithTagRemaining <= 0);
        }

        UpdateSpawnManagers(transforms, spawnManagers, delta);
    }  
}

void EnemySystem::SpawnEnemiesInScene(
    std::unordered_map<unsigned long long, Enemy>& enemies,
    std::unordered_map<unsigned long long, Transform>& transforms,
    bool spawner
)
{

}


bool EnemySystem::FileSelector(std::string* filename)
{
    std::string* selected = nullptr;
    std::vector<std::string> loadPaths = {};
    std::vector<std::string*> loadPathsPointers = {};
    loadPaths.clear();
    loadPathsPointers.clear();

    for (auto& i : std::filesystem::directory_iterator(Paths::systemPath))
    {
        loadPaths.push_back(i.path().generic_string().substr(Paths::systemPath.size()));
        if (loadPaths.back().substr(loadPaths.back().size() - Paths::enemySystemExtension.size()) != Paths::enemySystemExtension) {
            loadPaths.erase(--loadPaths.end());
            continue;
        }
        loadPaths.back() = loadPaths.back().substr(0, loadPaths.back().size() - Paths::enemySystemExtension.size());
    }
    for (auto& i : loadPaths)
    {
        loadPathsPointers.push_back(&i);
        if (*filename == i) {
            selected = &i;
        }
    }


    if (ExtraEditorGUI::InputSearchBox(loadPathsPointers.begin(), loadPathsPointers.end(), &selected, "Enemy System Filename", Utilities::PointerToString(filename))) {
        *filename = *selected;
        return true;
    }

    return false;
}

void EnemySystem::GUI()
{
    FileSelector(&filename);

    if (ImGui::Button("Save##EnemySystems")) {

        std::ofstream file(Paths::systemPath + filename + Paths::enemySystemExtension);

        toml::table table = Serialise();

        file << table << '\n';

        file.close();
    }
    ImGui::SameLine();
    if (ImGui::Button("Load##EnemySystems")) {
        std::ifstream file(Paths::systemPath + filename + Paths::enemySystemExtension);

        toml::table data = toml::parse(file);

        Load(data);
 
        file.close();
    }
    ImGui::SameLine();
    if (ImGui::Button("Save as"))
    {
        saveAs = true;
    }


    SaveAsGUI();

    ImGui::Checkbox("AI Updating",&aiUpdating);
    ImGui::SameLine();
    ImGui::Checkbox("Draw Force Lines", &drawForceLines);
    ImGui::Text("AI STATS");
    ImGui::DragFloat("Max Speed", &maxSpeed);
    ImGui::DragFloat("Alignment Coef", &alignmentCoef);
    ImGui::DragFloat("Cohesion Coef", &cohesionCoef);
    ImGui::DragFloat("Seperation Coef", &seperationCoef);
    ImGui::DragFloat("Normal Coef", &normalCoef);
    ImGui::DragFloat("Player Coef", &playerCoef);
    ImGui::DragFloat("Slow Down When Not Boiding Percentage", &slowedPercentage);

    ImGui::DragFloat("Perception Radius", &perceptionRadius);
    ImGui::DragFloat("Separation Radius", &separationRadius);

    ImGui::DragFloat("Time Till Enemies Spawn Again From Used Spawner", &timeForEnemiesToSpawnAgain);

    ImGui::Text("EXPLODING ENEMY STATS");
    ImGui::DragInt("Explosive Enemy Health", &explosiveEnemyHealth);
    ImGui::DragFloat("Time To Explode", &timeToExplode, 0.02f, 0);
    ImGui::DragFloat("Speed While Exploding", &speedWhileExploding, 2.0f, 0);
    ImGui::DragInt("Explosion Damage", &explosionDamage);
    ImGui::DragFloat("Explosion Radius", &explosionRadius, 25.0f, 0);
    ImGui::DragFloat("Distance To Start Exploding", &distanceToExplode, 25.0f, 0);
    ImGui::DragFloat("Explosive Enemy Collider Radius", &explosiveEnemyColliderRadius);


    ImGui::Text("MELEE ENEMY STATS");
    ImGui::DragInt("Melee Enemy Health", &meleeEnemyHealth);
    ImGui::DragInt("Melee Enemy Damage", &meleeEnemyDamage);
    ImGui::DragFloat("Melee Enemy Collider Radius", &meleeEnemyColliderRadius);
    ImGui::DragFloat("Distance To Start Punching", &distanceToPunch, 25.0f, 0);
    ImGui::InputText("Melee Enemy Material", &meleeEnemyMaterialPath);
    ImGui::Text("");
    ImGui::Text("RANGED ENEMY STATS");
    ImGui::DragInt("Ranged Enemy Health", &rangedEnemyHealth);
    ImGui::DragInt("Ranged Enemy Damage", &rangedEnemyDamage);
    ImGui::DragFloat("Ranged Enemy Collider Radius", &rangedEnemyColliderRadius);
    ImGui::DragFloat("Distance To Start Shooting", &distanceToShoot, 25.0f, 0);
    ImGui::DragFloat("Distance To Start Fleeing", &distanceToFlee, 25.0f, 0);
    ImGui::InputText("Ranged Enemy Material", &rangedEnemyMaterialPath);

    ImGui::End();
}

toml::table EnemySystem::Serialise() const
{
    return 
            toml::table{
                { "maxSpeed", maxSpeed },
                { "alignmentCoef", alignmentCoef },
                { "cohesionCoef", cohesionCoef },
                { "seperationCoef", seperationCoef },
                { "normalCoef", normalCoef },
                { "playerCoef", playerCoef },
                { "slowedPercentage", slowedPercentage },
                { "perceptionRadius", perceptionRadius },
                { "separationRadius", separationRadius },
                { "timeForEnemiesToSpawnAgain", timeForEnemiesToSpawnAgain },
                { "explosiveEnemyHealth", explosiveEnemyHealth },
                { "timeToExplode", timeToExplode },
                { "speedWhileExploding", speedWhileExploding },
                { "explosionDamage", explosionDamage },
                { "explosionRadius", explosionRadius },
                { "distanceToExplode", distanceToExplode },
                { "explosiveEnemyColliderRadius", explosiveEnemyColliderRadius },
                { "meleeEnemyHealth", meleeEnemyHealth },
                { "meleeEnemyDamage", meleeEnemyDamage },
                { "meleeEnemyColliderRadius", meleeEnemyColliderRadius },
                { "distanceToPunch", distanceToPunch },
                { "timeToPunch", timeToPunch },
                { "meleeEnemyModel", meleeEnemyModel },
                { "meleeEnemyMaterialPath", meleeEnemyMaterialPath },
                { "rangedEnemyHealth", rangedEnemyHealth },
                { "rangedEnemyDamage", rangedEnemyDamage },
                { "rangedEnemyColliderRadius", rangedEnemyColliderRadius },
                { "distanceToShoot", distanceToShoot },
                { "distanceToFlee", distanceToFlee },
                { "rangedEnemyModel", rangedEnemyModel },
                { "rangedEnemyMaterialPath", rangedEnemyMaterialPath },
    };
}
void EnemySystem::SaveAsGUI()
{
    if (saveAs)
    {
        ImGui::OpenPopup("Save Enemy System As");
        saveAs = false;
    }
    if (ImGui::BeginPopupModal("Save Enemy System As", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {

        ImGui::InputText("File ", &newFilename);

        if (ImGui::Button("Save"))
        {
            if (newFilename != "") {
                std::ofstream file(Paths::systemPath + newFilename + Paths::enemySystemExtension);
                filename = newFilename;
                toml::table table = Serialise();

                file << table << '\n';

                file.close();
                ImGui::CloseCurrentPopup();
                
                newFilename = "";
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            newFilename = "";
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}
void EnemySystem::UpdateNormalFlowMap(
    std::unordered_map<unsigned long long, Transform>& transforms,
    std::unordered_map<unsigned long long, RigidBody>& rigidBodies,
    std::unordered_map<unsigned long long, Collider*>& colliders
)
{
    int w, h, channels;
    unsigned char* nfmImage = stbi_load(
        (Paths::levelsPath + SceneManager::scene->windowName + Paths::imageExtension).c_str(),
        &w, &h, &channels, STBI_rgb_alpha
        );

    if (nfmImage)
    {
        LoadNormalFlowMapFromImage(nfmImage, w, h);
    }
    else
    {
        PopulateNormalFlowMap(transforms, rigidBodies, colliders);
    }
}

void EnemySystem::LoadNormalFlowMapFromImage(unsigned char* image, int width, int height)
{
    normalFlowMap.clear();
    normalFlowMap.reserve(width * height);
    mapDimensions.x = width;
    mapDimensions.y = height;
    for (int i = 0; i < width * height; i++)
    {
        glm::vec2 colour = glm::vec2(
            image[4 * i + 0]/255.0f, image[4 * i + 1]/255.0f
        );
        colour -= 0.5f;
        colour *= 2.0f;
        //colour *= image[4 * i + 2];
        normalFlowMap.push_back(colour);
    }

}

void EnemySystem::PopulateNormalFlowMap(
    std::unordered_map<unsigned long long, Transform>& transforms,
    std::unordered_map<unsigned long long, RigidBody>& rigidbodies,
    std::unordered_map<unsigned long long, Collider*>& colliders
)
{
    /*
    * For each rigidbody
    *   for each collider
    *     if poly
    *        for each normal
    *           for each pixel
    *               calculate influence
    *     if circle
    *        for distance from centre
    *           for each pixel
    *               calculate influence
    * 
    * save as image
    */


    std::vector<unsigned char> mapColours;
    mapColours.reserve(mapDimensions.x * mapDimensions.y * 4);
    std::vector<glm::vec3> normalFlowMapVec3;
    normalFlowMapVec3.reserve(mapDimensions.x * mapDimensions.y);

    for (int i = 0; i < mapDimensions.x * mapDimensions.y; i++)
    {
        normalFlowMapVec3.push_back({0.0f, 0.0f, FLT_MAX});
        mapColours.push_back(0);
        mapColours.push_back(0);
        mapColours.push_back(0);
        mapColours.push_back(255);
    }
    
    for (auto& rigidBodyPair : rigidbodies)
    {
        for (auto& collider : *rigidBodyPair.second.getColliders())
        {
            if ((collider->collisionLayer & ((int)CollisionLayers::base | (int)CollisionLayers::softCover)) && !collider->isTrigger)
            {
                if (collider->getType() == ColliderType::polygon)
                {
                    PolygonCollider* poly = (PolygonCollider*)collider;
                    
                    if (poly->verts.size() == 1)
                    {
                        PopulateNormalFlowMapFromCircle(poly, transforms[rigidBodyPair.first], normalFlowMapVec3);
                    }

                    else
                    {
                        PopulateNormalFlowMapFromPoly(poly, transforms[rigidBodyPair.first], normalFlowMapVec3);
                    }
                }
            }
        }
    }

    for (auto& colliderPair : colliders)
    {
        Collider* collider = colliderPair.second;
        if ((collider->collisionLayer & ((int)CollisionLayers::base | (int)CollisionLayers::softCover)) && !collider->isTrigger)
        {
            if (collider->getType() == ColliderType::polygon)
            {
                PolygonCollider* poly = (PolygonCollider*)collider;

                if (poly->verts.size() == 1)
                {
                    PopulateNormalFlowMapFromCircle(poly, transforms[colliderPair.first], normalFlowMapVec3);
                }

                else
                {
                    PopulateNormalFlowMapFromPoly(poly, transforms[colliderPair.first], normalFlowMapVec3);
                }
            }
        }
    }

    for (int x = 0; x < mapDimensions.x; x++)
    {

        for (int z = 0; z < mapDimensions.y; z++)
        {
            float r, g, b = 0.0f;
            glm::vec3 direction = normalFlowMapVec3[x + z * mapDimensions.x];

            b = Utilities::Lerp(1.0f, 0.0f, direction.z / (maxNormalInfluence));
            b = glm::clamp(b, 0.0f, 1.0f);
            glm::vec2 influence = glm::vec2(direction.x, direction.y);

            influence.x = fmin(influence.x, 1.0f);
            influence.x = fmax(influence.x, -1.0f);
            influence.y = fmin(influence.y, 1.0f);
            influence.y = fmax(influence.y, -1.0f);
            influence /= 2.0f;
            influence += 0.5f;
            influence *= 255;
            r = influence.x * b;
            g = influence.y * b;
            b *= 255.0f;

            mapColours[(x + z * mapDimensions.x) * 4 + 0] = r;
            mapColours[(x + z * mapDimensions.x) * 4 + 1] = g;
            mapColours[(x + z * mapDimensions.x) * 4 + 2] = b;
        }
    }

    stbi_write_png((Paths::levelsPath + SceneManager::scene->windowName + Paths::imageExtension).c_str(), mapDimensions.x, mapDimensions.y, 4, mapColours.data(),0);
    UpdateNormalFlowMap(transforms, rigidbodies, colliders);
}

void EnemySystem::PopulateNormalFlowMapFromCircle(PolygonCollider* circle, Transform& transform, std::vector<glm::vec3>& normalFlowMapVec3)
{
    glm::vec2 pos = RigidBody::Transform2Din3DSpace(transform.getGlobalMatrix(), circle->verts[0]);
    float radius = circle->radius;

    float minX = pos.x - radius;
    float minY = pos.y - radius;
    minX -= maxNormalInfluence;
    minY -= maxNormalInfluence;
    minX = fmaxf((float)mapMinCorner.x, minX);
    minY = fmaxf((float)mapMinCorner.y, minY);

    minX -= mapMinCorner.x;
    minX /= nfmDensity;
    minY -= mapMinCorner.y;
    minY /= nfmDensity;

    float maxX = pos.x + radius;
    float maxY = pos.y + radius;
    maxX += maxNormalInfluence;
    maxY += maxNormalInfluence;
    maxX = fminf((float)(mapMinCorner.x + mapDimensions.x * nfmDensity), maxX);
    maxY = fminf((float)(mapMinCorner.y + mapDimensions.y * nfmDensity), maxY);

    maxX -= mapMinCorner.x;
    maxX /= nfmDensity;
    maxY -= mapMinCorner.y;
    maxY /= nfmDensity;

    //Check every "tile" on colourMap
    for (int x = minX; x < maxX; x++)
    {
        for (int z = minY; z < maxY; z++)
        {
            glm::vec2 tilePos = { x * nfmDensity + mapMinCorner.x, z * nfmDensity + mapMinCorner.y };
            glm::vec2 delta = tilePos - pos;
            float distance = glm::dot(delta, delta);
            if (distance >= radius * radius)
            {
                distance = sqrtf(distance);
                if (distance < normalFlowMapVec3[x + z * mapDimensions.x].z)
                {
                    glm::vec2 normal = glm::normalize(delta);
                    normalFlowMapVec3[x + z * mapDimensions.x] = glm::vec3(normal.x, normal.y, distance);
                }
            }
        }
    }
}

void EnemySystem::PopulateNormalFlowMapFromPoly(PolygonCollider* poly, Transform& transform, std::vector<glm::vec3>& normalFlowMapVec3)
{
    for (int i = 0; i < poly->verts.size(); i++)
    {
        glm::vec2 vertA = RigidBody::Transform2Din3DSpace(transform.getGlobalMatrix(), poly->verts[i]);
        glm::vec2 vertB = RigidBody::Transform2Din3DSpace(transform.getGlobalMatrix(), poly->verts[(i + 1) % poly->verts.size()]);

        glm::vec2 seperation = vertB - vertA;
        if (glm::dot(seperation, seperation) < 1000)
            continue;
        glm::vec2 tangent = glm::normalize(seperation);

        float tanVertA = glm::dot(vertA, tangent);
        float tanVertB = glm::dot(vertB, tangent);

        glm::vec2 normal = { -tangent.y, tangent.x };
        float comparisonNormal = glm::dot(vertB, normal);

        float minX = fminf(vertA.x, vertB.x);
        float minY = fminf(vertA.y, vertB.y);
        minX -= maxNormalInfluence;
        minY -= maxNormalInfluence;
        minX = fmaxf((float)mapMinCorner.x, minX);
        minY = fmaxf((float)mapMinCorner.y, minY);

        minX -= mapMinCorner.x;
        minX /= nfmDensity;
        minY -= mapMinCorner.y;
        minY /= nfmDensity;

        float maxX = fmaxf(vertA.x, vertB.x);
        float maxY = fmaxf(vertA.y, vertB.y);
        maxX += maxNormalInfluence;
        maxY += maxNormalInfluence;
        maxX = fminf((float)(mapMinCorner.x + mapDimensions.x * nfmDensity), maxX);
        maxY = fminf((float)(mapMinCorner.y + mapDimensions.y * nfmDensity), maxY);

        maxX -= mapMinCorner.x;
        maxX /= nfmDensity;
        maxY -= mapMinCorner.y;
        maxY /= nfmDensity;

        for (int x = (int)minX; x < maxX; x++)
        {
            for (int z = (int)minY; z < maxY; z++)
            {
                glm::vec2 tilePos = { x * nfmDensity + mapMinCorner.x, z * nfmDensity + mapMinCorner.y };
                float normalDot = glm::dot(tilePos, normal);
                if (normalDot >= comparisonNormal)
                {
                    float tileSeperationDot = glm::dot(tilePos, tangent);
                    float distance = normalDot - comparisonNormal;
                    if (tileSeperationDot > tanVertA && tileSeperationDot < tanVertB)
                    {
                        if (distance <= normalFlowMapVec3[x + z * mapDimensions.x].z)
                            normalFlowMapVec3[x + z * mapDimensions.x] = glm::vec3(normal.x, normal.y, distance);
                    }

                    else if (tileSeperationDot >= tanVertA - distance && tileSeperationDot <= tanVertB + distance)
                    {
                        float distanceToA = glm::length(tilePos - vertA);
                        float distanceToB = glm::length(tilePos - vertB);
                        glm::vec2 thisNormal;
                        if (distanceToA < distanceToB)
                        {
                            thisNormal = glm::normalize(tilePos - vertA);
                            distance = distanceToA;
                        }
                        else
                        {
                            thisNormal = glm::normalize(tilePos - vertB);
                            distance = distanceToB;
                        }
                        if (distance <= normalFlowMapVec3[x + z * mapDimensions.x].z)
                            normalFlowMapVec3[x + z * mapDimensions.x] = glm::vec3(thisNormal.x, thisNormal.y, distance);
                    }
                }
            }
        }
    }
}

void EnemySystem::AddTag(std::string tag, int count)
{
    enemyTags[tag] = count;
}

