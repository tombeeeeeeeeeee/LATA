#include "Sync.h"

#include "Transform.h"
#include "RigidBody.h"
#include "SceneObject.h"
#include "Health.h"
#include "ModelRenderer.h"
#include "LineRenderer.h"
#include "ResourceManager.h"
#include "shaderEnum.h"
#include "EditorGUI.h"
#include "SceneManager.h"
#include "PhysicsSystem.h"
#include "Hit.h"

void Sync::Start(std::vector<Shader*>* shaders)
{
	Model* misfireModel = ResourceManager::LoadModel(misfireModelPath);
	Material* misfireMaterial = ResourceManager::LoadMaterial("misfireProjectile", (*shaders)[super]);
	misfireMaterial->AddTextures({
		ResourceManager::LoadTexture("images/defaultTexture.png", Texture::Type::albedo, GL_REPEAT)
		});
	misfireModelRender = new ModelRenderer(misfireModel, misfireMaterial);
}

void Sync::Update(
	Input::InputDevice& inputDevice, Transform& transform,
	RigidBody& rigidBody, LineRenderer* lines, 
	float delta, float cameraAngleOffset
)
{
	glm::vec2 look = inputDevice.getLook();
	glm::vec2 move = inputDevice.getMove();

	if (glm::length(move) > moveDeadZone)
	{
		float c = cosf(cameraAngleOffset * PI / 180.0f);
		float s = sinf(cameraAngleOffset * PI / 180.0f);
		move = {
			move.x * c - move.y * s,
			move.x * s + move.y * c
		};
		rigidBody.vel = moveSpeed * move;
		fireDirection = move;
	}
	else
	{
		rigidBody.vel = { 0.0f, 0.0f };
	}

	if (glm::length(look) > lookDeadZone)
	{
		float c = cosf(cameraAngleOffset * PI / 180.0f);
		float s = sinf(cameraAngleOffset * PI / 180.0f);
		fireDirection = {
			look.x * c - look.y * s,
			look.x * s + look.y * c
		};
	}
	fireDirection = glm::normalize(fireDirection);

	if (inputDevice.getRightTrigger())
	{
		//Begin Chagrging Shot
		if (!chargingShot)
		{
			chargingShot = true;
			chargedDuration = 0.0f;
		}

		//Charging shot.
		if (chargedDuration < sniperChargeTime && currCharge > sniperChargeCost)
		{
			if (chargedDuration + delta > sniperChargeTime)
			{
				//Do charging stuff
			}
			chargedDuration += delta;
		}
		else if (chargedDuration < overclockChargeTime && currCharge > overclockChargeCost)
		{
			if (chargedDuration + delta > overclockChargeTime) 
			{
			//Do charging stuff
			}
			chargedDuration += delta;
		}
	
		//TODO: add rumble
	}
	else if (chargingShot)
	{
		chargingShot = false;
		if (chargedDuration >= overclockChargeTime)
		{
			ShootOverClocked(transform.getGlobalPosition());
		}
		else if (chargedDuration >= sniperChargeTime)
		{
			ShootSniper(transform.getGlobalPosition());
		}
		else
		{
			ShootMisfire(transform);
		}
	}

	for (auto i = blasts.begin(); i != blasts.end();)
	{
		i->timeElapsed += delta;
		float r = i->colour.x - i->colour.x * i->timeElapsed / i->lifeSpan;
		float g = i->colour.y - i->colour.y * i->timeElapsed / i->lifeSpan;
		float b = i->colour.z - i->colour.z * i->timeElapsed / i->lifeSpan;
		lines->DrawLineSegment(i->startPosition, i->endPosition, {r,g,b});
		if (i->timeElapsed > i->lifeSpan)
			i = blasts.erase(i);
		else
			++i;
	}
}

void Sync::GUI()
{
	//ImGui::Text("");
	if (ImGui::CollapsingHeader("Sync Component"))
	{
		ImGui::DragFloat("Move Speed", &moveSpeed);
		ImGui::DragFloat("Look DeadZone", &lookDeadZone);
		ImGui::DragFloat("Move DeadZone", &moveDeadZone);
		ImGui::DragFloat("Misfire Damage", &misfireDamage);
		ImGui::DragFloat("Misfire Charge Cost", &misfireChargeCost);
		ImGui::DragFloat("Sniper Damage", &sniperDamage);
		ImGui::DragFloat("Sniper Charge Cost", &sniperChargeCost);
		ImGui::DragFloat("Sniper Charge Time", &sniperChargeTime);
		ImGui::DragFloat("Overclock Damage", &overclockDamage);
		ImGui::DragFloat("Overclock Charge Cost", &overclockChargeCost);
		ImGui::DragFloat("Overlock Charge Time", &overclockChargeTime);

	}
}

void Sync::ShootMisfire(Transform& transform)
{
	currCharge -= misfireChargeCost;
	SceneObject* shot = new SceneObject(SceneManager::scene);
	shot->setRenderer(misfireModelRender);

	shot->setRigidBody(new RigidBody(0.0f,0.0f));
	PolygonCollider collider = PolygonCollider({ {0.0f,0.0f} }, misfireColliderRadius);
	collider.collisionLayer = (int)CollisionLayers::sync;
	collider.isTrigger -= true;
	shot->rigidbody()->addCollider(&collider);
	shot->rigidbody()->AddImpulse(fireDirection * misfireShotSpeed);
	shot->rigidbody()->onTrigger.push_back([this](Collision collision) {misfireShotOnCollision(collision); });

	float angle = acosf(glm::dot(fireDirection, { barrelOffset.x, barrelOffset.z }) / glm::length(glm::vec2(barrelOffset.x, barrelOffset.z)));
	float turnSign = glm::sign(glm::dot(fireDirection, { transform.right().x, transform.right().z }));
	float c = cos(turnSign * angle);
	float s = sin(turnSign * angle);
	glm::vec3 globalBarrelOffset = {
		barrelOffset.x * c - barrelOffset.z * s,
		0.0f,
		barrelOffset.x * s + barrelOffset.z * c
	};
	shot->transform()->setPosition(transform.getGlobalPosition() + globalBarrelOffset);

}

void Sync::ShootSniper(glm::vec3 pos)
{
	currCharge -= sniperChargeCost;
	std::vector<Hit> hits;
	PhysicsSystem::RayCast({ pos.x, pos.z }, fireDirection, hits, FLT_MAX, ~(int)CollisionLayers::sync);
	Hit hit = hits[0];
	blasts.push_back({ sniperBeamLifeSpan, 0.0f, sniperBeamColour, pos, {hit.position.x, pos.y, hit.position.y} });
	if (hit.collider->collisionLayer & (int)CollisionLayers::enemy)
	{
		hit.sceneObject->health()->subtractHealth(sniperDamage);
	}
}

void Sync::ShootOverClocked(glm::vec3 pos)
{
	currCharge -= overclockChargeCost;
	OverclockRebounding(pos, fireDirection, 0, overclockBeamColour);
}

void Sync::OverclockRebounding(glm::vec3 pos, glm::vec2 dir, int count, glm::vec3 colour)
{
	std::vector<Hit> hits;
	if (PhysicsSystem::RayCast({ pos.x, pos.z }, dir, hits, FLT_MAX, ~(int)CollisionLayers::sync))
	{
		Hit hit = hits[0];

		if (hit.collider->collisionLayer & (int)CollisionLayers::enemy)
		{
			hit.sceneObject->health()->subtractHealth(sniperDamage);
			for (int i = 0; i < hits.size() && i < enemyPierceCount; i++)
			{
				hit = hits[i];
				if (i == enemyPierceCount - 1) return;
				if (hit.collider->collisionLayer & (int)CollisionLayers::enemy)
				{
					hit.sceneObject->health()->subtractHealth(sniperDamage);
				}
				else break;
			}
		}

		blasts.push_back({ overclockBeamLifeSpan, 0.0f, colour, pos, {hit.position.x, pos.y, hit.position.y} });

		if (hit.collider->collisionLayer & (int)CollisionLayers::ecco)
		{
			float s = 0.95f;
			float v = 0.95f;
			float angle = -eccoRefractionAngle * eccoRefractionCount / 2.0f;
			for (int iter = 0; iter < eccoRefractionCount; iter++)
			{
				float h = iter / (float)eccoRefractionCount;
				int i = int(h * 6.0);
				float f = h * 6.0 - i;
				float w = v * (1.0 - s);
				float q = v * (1.0 - s * f);
				float t = v * (1.0 - s * (1.0 - f));

				glm::vec3 refractionColour;
				if (i == 0) refractionColour = { v, t, w };
				else if (i == 1) refractionColour = { q, v, w };
				else if (i == 2) refractionColour = { w, v, t }; 
				else if (i == 3) refractionColour = { w, q, v };
				else if (i == 4) refractionColour = { t, w, v };
				else		   refractionColour = { v, w, q };

				float c = cosf(angle * PI / 180.0f);
				float s = sinf(angle * PI / 180.0f);

				glm::vec2 refractionDirection =
				{
					dir.x * c - dir.y * s,
					dir.x * s + dir.y * c
				};

				OverclockNonRebounding({ hit.position.x, pos.y, hit.position.y }, refractionDirection, refractionColour);
				angle += eccoRefractionAngle;
			}
		}
		else if (count < overclockReboundCount)
		{
			{
				OverclockRebounding({ hit.position.x,pos.y, hit.position.y }, glm::reflect(dir, hit.normal), count + 1, colour);
			}
		}
	}
}

void Sync::OverclockNonRebounding(glm::vec3 pos, glm::vec2 dir, glm::vec3 colour)
{
	std::vector<Hit> hits;
	PhysicsSystem::RayCast({ pos.x, pos.z }, dir, hits, FLT_MAX, ~(int)CollisionLayers::sync);
	Hit hit = hits[0];

	if (hit.collider->collisionLayer & (int)CollisionLayers::enemy)
	{
		hit.sceneObject->health()->subtractHealth(sniperDamage);
	}
	if((hit.collider->collisionLayer & (int)CollisionLayers::enemy) | (hit.collider->collisionLayer & (int)CollisionLayers::ecco))
	{
		for (int i = 0; i < hits.size() && i < enemyPierceCount; i++)
		{
			hit = hits[i];
			if (i == enemyPierceCount - 1) return;
			if (hit.collider->collisionLayer & (int)CollisionLayers::enemy)
			{
				hit.sceneObject->health()->subtractHealth(sniperDamage);
			}
			else break;
		}
	}

	blasts.push_back({ overclockBeamLifeSpan, 0.0f, colour, pos, {hit.position.x, pos.y, hit.position.y} });
	return;
}

void Sync::misfireShotOnCollision(Collision collision)
{
	if (collision.collisionMask & (unsigned int)CollisionLayers::enemy)
	{
		collision.sceneObject->health()->subtractHealth(misfireDamage);
	}
}

void Sync::sniperShotOnCollision(Collision collision)
{
	if (collision.collisionMask & (unsigned int)CollisionLayers::enemy)
	{
		collision.sceneObject->health()->subtractHealth(sniperDamage);
	}
}

void Sync::overclockShotOnCollision(Collision collision)
{
	if (collision.collisionMask & (unsigned int)CollisionLayers::enemy)
	{
		collision.sceneObject->health()->subtractHealth(overclockDamage);
	}
}

