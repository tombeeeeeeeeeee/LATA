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
#include "Paths.h"
#include "stb_image.h"
#include "stb_image_write.h"


EnemySystem::EnemySystem(toml::table table)
{
    meleeEnemyHealth = Serialisation::LoadAsInt(table["meleeEnemyHealth"]);
    meleeEnemyDamage = Serialisation::LoadAsInt(table["meleeEnemyDamage"]);
    meleeEnemyColliderRadius = Serialisation::LoadAsFloat(table["meleeEnemyColliderRadius"]);
    meleeEnemyModel = Serialisation::LoadAsString(table["meleeEnemyModel"]);
    meleeEnemyMaterialPath = Serialisation::LoadAsString(table["meleeEnemyMaterialPath"]);

    rangedEnemyHealth = Serialisation::LoadAsInt(table["rangedEnemyHealth"]);
    rangedEnemyDamage = Serialisation::LoadAsInt(table["rangedEnemyDamage"]);
    rangedEnemyColliderRadius = Serialisation::LoadAsFloat(table["rangedEnemyColliderRadius"]);
    rangedEnemyModel = Serialisation::LoadAsString(table["rangedEnemyModel"]);
    rangedEnemyMaterialPath = Serialisation::LoadAsString(table["rangedEnemyMaterialPath"]);

    glm::vec3 offscreenSpawnPosition = Serialisation::LoadAsVec3(table["offscreenSpawnPosition"]);
}

void EnemySystem::Start(
    std::unordered_map<unsigned long long, Transform>& transforms,
    std::unordered_map<unsigned long long, RigidBody>& rigidbodies,
    std::unordered_map<unsigned long long, Collider*>& colliders
)
{
    if (!explosiveEnemyRenderer)
    {
        Material* explosiveEnemyMaterial = ResourceManager::defaultMaterial;

        explosiveEnemyRenderer = new ModelRenderer(
            ResourceManager::LoadModelAsset(Paths::modelSaveLocation + explosiveEnemyModel + Paths::modelExtension),
            explosiveEnemyMaterial
        );
    }

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

    UpdateNormalFlowMap(transforms, rigidbodies, colliders);
}

void EnemySystem::SpawnExplosive(glm::vec3 pos)
{
    Scene* scene = SceneManager::scene;

    SceneObject* enemy = new SceneObject(scene, "ExplosiveEnemy" + std::to_string(explosiveEnemyCount++));
    enemy->setEnemy(new Enemy((int)EnemyType::explosive));
    enemy->setHealth(new Health());
    enemy->health()->setMaxHealth(explosiveEnemyHealth);
    enemy->setRigidBody(new RigidBody());
    enemy->rigidbody()->invMass = 1.0f;
    enemy->setRenderer(
        explosiveEnemyRenderer
    );
    enemy->transform()->setParent(nullptr);
    enemy->transform()->setPosition(pos);
    enemy->transform()->setScale(6.0f);
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



void EnemySystem::LineOfSightAndTargetCheck(
    Enemy enemy, Transform transform, RigidBody rigidBody,
    SceneObject* ecco, SceneObject* sync
)
{

    glm::vec2 syncPos2D = { sync->transform()->getGlobalPosition().x, sync->transform()->getGlobalPosition().z };
    glm::vec2 eccoPos2D = { ecco->transform()->getGlobalPosition().x, ecco->transform()->getGlobalPosition().z };
    enemy.hasLOS = false;
    glm::vec2 enemyPos2D = transform.get2DGlobalPosition();

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
            enemy.lastTargetPos = syncPos2D;
            enemy.hasLOS = true;
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
                enemy.lastTargetPos = eccoPos2D;
                enemy.hasLOS = true;
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
                    SceneManager::scene->DeleteSceneObject(enemyPair.first);
                }
                break;
            case (int)EnemyType::melee:
                enemyPair.second.inAbility = false;
                glm::vec3 f = transforms[enemyPair.first].forward();

                hits = PhysicsSystem::CircleCast(enemyPos + glm::vec2(f.x * meleeEnemyColliderRadius, f.z * meleeEnemyColliderRadius), punchRadius, (int)CollisionLayers::sync | (int)CollisionLayers::ecco);
                for (auto& hit : hits)
                {
                    Health* health = hit.sceneObject->health();
                    if (health)
                    {
                        health->subtractHealth(meleeEnemyDamage);
                    }
                }
                break;
            case (int)EnemyType::ranged:
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
            float length = glm::length(rigidBodies[enemyPair.first].vel);
            float slowedLength = length * slowedPercentage / 100;

            enemyPair.second.influenceThisFrame = -glm::normalize(rigidBodies[enemyPair.first].vel) * fminf(length, slowedLength);
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
                        float distance = glm::length(otherToSelf);
                        float strength = 1.0f - (distance / separationRadius);

                        enemyPair.second.influenceThisFrame += (otherToSelf / distance) * strength * seperationCoef;
                    }
                }
            }



            float sqrDistanceToEcco = glm::dot(eccoPos - enemyPos, eccoPos - enemyPos);
            float sqrDistanceToSync = glm::dot(syncPos - enemyPos, syncPos - enemyPos);
            glm::vec2 playerForce;
            if (sqrDistanceToEcco < sqrDistanceToSync)
            {
                playerForce = glm::normalize(eccoPos - enemyPos);
            }
            else
            {
                playerForce = glm::normalize(syncPos - enemyPos);
            }
            playerForce *= 100 * playerCoef;

            glm::vec2 normalForce = GetNormalFlowInfluence(enemyPos) * normalCoef;
            enemyPair.second.influenceThisFrame += normalForce + playerForce;

            //RenderSystem::lines.DrawLineSegement2D(enemyPos, enemyPos + normalForce, {1,0,0}, 100);
            //RenderSystem::lines.DrawCircle({ enemyPos.x, 100, enemyPos.y }, 20, { 1,0,0 });
            if (totalNeighbours == 0) continue;

            avgPos /= totalNeighbours;
            avgVel /= totalNeighbours;
            //Todo: enemyPair.second.lastTargetPos

            glm::vec2 alignmentForce = avgVel * alignmentCoef;
            glm::vec2 cohesionForce = (avgPos - enemyPos) * cohesionCoef;
            enemyPair.second.influenceThisFrame += alignmentForce + cohesionForce;
        }
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

        float angle = atan2f(rigidBodies[enemyPair.first].vel.x, rigidBodies[enemyPair.first].vel.y) * 180.0f / PI;
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
                SceneManager::scene->DeleteSceneObject(enemyPair.first);
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


//TODO: Add AI Pathfinding and attacking in here.
void EnemySystem::Update   (
    std::unordered_map<unsigned long long, Enemy>& enemies,
    std::unordered_map<unsigned long long, Transform>& transforms, 
    std::unordered_map<unsigned long long, RigidBody>& rigidbodies,
    std::unordered_map<unsigned long long, Health>& healths,
    glm::vec2 eccoPos, glm::vec2 syncPos, float delta
)
{
    if (aiUpdating)
    {
        AbilityCheck(
            enemies,
            rigidbodies,
            transforms,
            eccoPos, syncPos, delta
        );

        Steering(
            enemies, 
            transforms, 
            rigidbodies, 
            eccoPos, syncPos,
            delta
        );
        
        HealthCheck(enemies, healths);
        
    }  
}

void EnemySystem::SpawnEnemiesInScene(
    std::unordered_map<unsigned long long, Enemy>& enemies,
    std::unordered_map<unsigned long long, Transform>& transforms,
    bool spawner
)
{
    for (auto& enemyPair : enemies)
    {
        if ((enemyPair.second.type & (int)EnemyType::spawnSpot))
        {
            if (enemyPair.second.type & (int)EnemyType::melee)
                SpawnMelee(transforms[enemyPair.first].getGlobalPosition());
            else if (enemyPair.second.type & (int)EnemyType::ranged)
                SpawnRanged(transforms[enemyPair.first].getGlobalPosition());
            else if (enemyPair.second.type & (int)EnemyType::explosive)
                SpawnExplosive(transforms[enemyPair.first].getGlobalPosition());

            SceneManager::scene->DeleteSceneObject(enemyPair.first);
        }
    }
}


void EnemySystem::GUI()
{
    ImGui::Checkbox("AI Updating",&aiUpdating);

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

    ImGui::Text("EXPLODING ENEMY STATS");
    ImGui::DragInt("Explosive Enemy Health", &explosiveEnemyHealth);
    ImGui::DragFloat("Time To Explode", &timeToExplode, 0.02f, 0);
    ImGui::DragInt("Explosion Damage", &explosionDamage);
    ImGui::DragFloat("Explosion Radius", &explosionRadius, 25.0f, 0);
    ImGui::DragFloat("Distance To Start Exploding", &distanceToExplode, 25.0f, 0);
    ImGui::DragFloat("Explosive Enemy Collider Radius", &explosiveEnemyColliderRadius);


    ImGui::Text("MELEE ENEMY STATS");
    ImGui::DragInt("Melee Enemy Health", &meleeEnemyHealth);
    ImGui::DragInt("Melee Enemy Damage", &meleeEnemyDamage);
    ImGui::DragFloat("Melee Enemy Collider Radius", &meleeEnemyColliderRadius);
    ImGui::DragFloat("Distance To Start Punching", &distanceToPunch, 25.0f, 0);
    if (ResourceManager::ModelSelector("Melee Enemy Model", &(meleeEnemyRenderer->model)))
    {
        meleeEnemyModel = meleeEnemyRenderer->model->path;
    }
    ImGui::InputText("Melee Enemy Material", &meleeEnemyMaterialPath);
    ImGui::Text("");
    ImGui::Text("RANGED ENEMY STATS");
    ImGui::DragInt("Ranged Enemy Health", &rangedEnemyHealth);
    ImGui::DragInt("Ranged Enemy Damage", &rangedEnemyDamage);
    ImGui::DragFloat("Ranged Enemy Collider Radius", &rangedEnemyColliderRadius);
    ImGui::DragFloat("Distance To Start Shooting", &distanceToShoot, 25.0f, 0);
    ImGui::DragFloat("Distance To Start Fleeing", &distanceToFlee, 25.0f, 0);
    if (ResourceManager::ModelSelector("Ranged Enemy Model", &(rangedEnemyRenderer->model)))
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
        { "meleeEnemyDamage", meleeEnemyDamage },
        { "meleeEnemyColliderRadius", meleeEnemyColliderRadius },
        { "meleeEnemyModel", meleeEnemyModel },
        { "meleeEnemyMaterialPath", meleeEnemyMaterialPath },
        { "rangedEnemyHealth", rangedEnemyHealth },
        { "rangedEnemyDamage", rangedEnemyDamage }, 
        { "rangedEnemyColliderRadius", rangedEnemyColliderRadius },
        { "rangedEnemyModel", rangedEnemyModel },
        { "rangedEnemyMaterialPath", rangedEnemyMaterialPath },
    };
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
        colour *= image[4 * i + 2];
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
        for (auto& collider : rigidBodyPair.second.colliders)
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
            r = influence.x;
            g = influence.y;
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

                    else if (tileSeperationDot > tanVertA - distance && tileSeperationDot < tanVertB + distance)
                    {
                        distance = distance * 1.414f;
                        if (distance <= normalFlowMapVec3[x + z * mapDimensions.x].z)
                            normalFlowMapVec3[x + z * mapDimensions.x] = glm::vec3(normal.x, normal.y, distance);
                    }
                }
            }
        }
    }
}

