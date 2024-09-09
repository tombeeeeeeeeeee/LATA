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

void Sync::Update(Input::InputDevice& inputDevice, Transform& transform, RigidBody& rigidBody, LineRenderer* lines, float delta)
{
	glm::vec2 look = inputDevice.getLook();
	glm::vec2 move = inputDevice.getMove();

	if (glm::length(move) > moveDeadZone)
	{
		rigidBody.vel = moveSpeed * move;
		fireDirection = move;
	}
	else
	{
		rigidBody.vel = { 0.0f, 0.0f };
	}

	if (glm::length(look) > lookDeadZone)
	{
		float turnAmount = glm::dot(transform.forward(), glm::normalize(glm::vec3(look.x, 0.0f, look.y)));
		float desiredAngle = glm::acos(turnAmount);
		glm::vec3 eulers = transform.getEulerRotation();
		transform.setEulerRotation({ eulers.x, eulers.y + desiredAngle, eulers.z });
		fireDirection = look;
	}


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

	for (auto i = blasts.begin(); i < blasts.end(); i++)
	{
		i->timeElapsed += delta;
		float r = i->colour.x - i->colour.x * i->timeElapsed / i->endTime;
		float g = i->colour.y - i->colour.y * i->timeElapsed / i->endTime;
		float b = i->colour.z - i->colour.z * i->timeElapsed / i->endTime;
		lines->DrawLineSegment(i->startPosition, i->endPosition, {r,g,b});
		if (i->timeElapsed > i->endTime) blasts.erase(i);
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
	Collider collider = PolygonCollider({ {0.0f,0.0f} }, misfireColliderRadius);
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
	Hit hit;
	PhysicsSystem::RayCast({pos.x, pos.z}, fireDirection, hit, FLT_MAX, ~(int)CollisionLayers::sync);
	blasts.push_back({});
}

void Sync::ShootOverClocked(glm::vec3 pos)
{
	currCharge -= overclockChargeCost;
}

void Sync::ShootOverClockedSplit(glm::vec3 pos, glm::vec3 dir, int num)
{
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

