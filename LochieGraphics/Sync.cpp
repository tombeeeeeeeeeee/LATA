#include "Sync.h"

#include "Transform.h"
#include "RigidBody.h"
#include "SceneObject.h"
#include "Health.h"
#include "ModelRenderer.h"
#include "LineRenderer.h"
#include "ShaderEnum.h"
#include "Hit.h"
#include "Paths.h"
#include "Collision.h"
#include "Collider.h"
#include "Scene.h"
#include "PhysicsSystem.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "Utilities.h"
#include "Ecco.h"

#include "EditorGUI.h"

#include "Serialisation.h"

Sync::Sync(toml::table table)
{
	GUID = Serialisation::LoadAsUnsignedLongLong(table["guid"]);
	moveSpeed = Serialisation::LoadAsFloat(table["moveSpeed"]);
	lookDeadZone = Serialisation::LoadAsFloat(table["lookDeadZone"]);
	moveDeadZone = Serialisation::LoadAsFloat(table["moveDeadZone"]);
	barrelOffset = Serialisation::LoadAsVec3(table["barrelOffset"]);
	misfireDamage = Serialisation::LoadAsInt(table["misfireDamage"]);
	misfireShotSpeed = Serialisation::LoadAsFloat(table["misfireShotSpeed"]);
	sniperDamage = Serialisation::LoadAsInt(table["sniperDamage"]);
	sniperChargeTime = Serialisation::LoadAsFloat(table["sniperChargeTime"]);
	sniperBeamLifeSpan = Serialisation::LoadAsFloat(table["sniperBeamLifeSpan"]);
	sniperBeamColour = Serialisation::LoadAsVec3(table["sniperBeamColour"]);
	overclockDamage = Serialisation::LoadAsInt(table["overclockDamage"]);
	overclockChargeTime = Serialisation::LoadAsFloat(table["overclockChargeTime"]);
	overclockBeamLifeSpan = Serialisation::LoadAsFloat(table["overclockBeamLifeSpan"]);
	overclockBeamColour = Serialisation::LoadAsVec3(table["overclockBeamColour"]);
	overclockReboundCount = Serialisation::LoadAsInt(table["overclockReboundCount"]);
	enemyPierceCount = Serialisation::LoadAsInt(table["enemyPierceCount"]);
	eccoRefractionAngle = Serialisation::LoadAsFloat(table["eccoRefractionAngle"]);
	eccoRefractionCount = Serialisation::LoadAsInt(table["eccoRefractionCount"]);
	misfireColliderRadius = Serialisation::LoadAsFloat(table["misfireColliderRadius"]);
	maxMoveForce = Serialisation::LoadAsFloat(table["maxMoveForce"]);
	maxStopForce = Serialisation::LoadAsFloat(table["maxStopForce"]);
	healthOffsetX = Serialisation::LoadAsFloat(table["healthOffsetX"], 0.618f);
	healthOffsetY = Serialisation::LoadAsFloat(table["healthOffsetY"], -0.845f);
	healthScaleX = Serialisation::LoadAsFloat(table["healthScaleX"], 0.304f);
	healthScaleY = Serialisation::LoadAsFloat(table["healthScaleY"], 0.077f);
	chargeOffsetX = Serialisation::LoadAsFloat(table["chargeOffsetX"], 0.618f);
	chargeOffsetY = Serialisation::LoadAsFloat(table["chargeOffsetY"], -0.952f);
	chargeScaleX = Serialisation::LoadAsFloat(table["chargeScaleX"], 0.304f);
	chargeScaleY = Serialisation::LoadAsFloat(table["chargeScaleY"], 0.024f);
	healthBackgroundColour = Serialisation::LoadAsVec3(table["healthBackgroundColour"], glm::vec3(0.05f, 0.67f, 0.0f));
	healthForegroundColour = Serialisation::LoadAsVec3(table["healthForegroundColour"], glm::vec3(0.1f, 1.0f, 0.0f));
	chargeBackgroundColour = Serialisation::LoadAsVec3(table["chargeBackgroundColour"], glm::vec3(0.53f, 0.0f, 0.08f));
	chargeForegroundColour = Serialisation::LoadAsVec3(table["chargeForegroundColour"], glm::vec3(0.93f, 0.11f, 0.14f));
}

void Sync::Start(
	std::vector<Shader*>* shaders
)
{
	Model* misfireModel = ResourceManager::LoadModelAsset(Paths::modelSaveLocation + misfireModelPath + Paths::modelExtension);
	Material* misfireMaterial = ResourceManager::defaultMaterial;
	misfireModelRender = new ModelRenderer(misfireModel, misfireMaterial);
}

bool Sync::Update(
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
		float currentMoveSpeed = Utilities::Lerp(moveSpeed, 0.0f, glm::min(chargedDuration, sniperChargeTime) / sniperChargeTime);
		
		glm::vec2 desiredVel = currentMoveSpeed * move;
		glm::vec2 desiredVelChange = desiredVel - rigidBody.vel;
		glm::vec2 force = desiredVelChange * rigidBody.getMass() / delta;
		if (glm::length(force) > maxMoveForce) {
			force = glm::normalize(force) * maxMoveForce;
		}
		rigidBody.netForce += force;

		// TODO: Walking sound

		fireDirection = move;
	}
	else
	{
		glm::vec2 desiredVelChange = -rigidBody.vel;
		glm::vec2 force = desiredVelChange * rigidBody.getMass() / delta;
		if (glm::length(force) > maxStopForce) {
			force = glm::normalize(force) * maxStopForce;
		}
		rigidBody.netForce += force;
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

	float angle = atan2f(fireDirection.x, fireDirection.y) * 180.0f/PI + 90.0f;
	glm::vec3 eulers = transform.getEulerRotation();
	eulers.y = angle;
	transform.setEulerRotation(eulers);

	timeSinceHealButtonPressed += delta;
	if (inputDevice.getButton2())
	{
		timeSinceHealButtonPressed = 0.0f;
	}

	globalBarrelOffset = barrelOffset;
	glm::vec2 barrelOffset2D = RigidBody::Transform2Din3DSpace(transform.getGlobalMatrix(), { barrelOffset.x, barrelOffset.z });
	globalBarrelOffset = { barrelOffset2D.x, barrelOffset.y, barrelOffset2D.y };

	if (inputDevice.getRightTrigger())
	{
		//Begin Chagrging Shot
		if (!chargingShot)
		{
			chargingShot = true;
			chargedDuration = 0.0f;
		}

		else
		{
			if (chargedDuration + delta >= sniperChargeTime && chargedDuration < sniperChargeTime)
			{
				//At this time the charge is enough to shoot the sniper.
				if (!playedReachCharge1) {
					playedReachCharge1 = true;
					SceneManager::scene->audio.PlaySound(Audio::railgunFirstChargeReached);
				}
			}
			else if (chargedDuration + delta >= overclockChargeTime && chargedDuration < overclockChargeTime)
			{
				//At this time the charge is enough to shoot the reflecting shot
				if (!playedReachCharge2) {
					playedReachCharge2 = true;
					SceneManager::scene->audio.PlaySound(Audio::railgunSecondChargeReached);
				}
			}
			chargedDuration += delta;
		}
		//TODO: add rumble
		glm::vec2 pos2D = RigidBody::Transform2Din3DSpace(transform.getGlobalMatrix(), { 0,0 });
		lines->DrawCircle(glm::vec3(pos2D.x, 0.1f, pos2D.y), 100.0f * glm::clamp(0.0f, chargedDuration / sniperChargeTime, 1.0f), { sniperBeamColour.x, sniperBeamColour.y, sniperBeamColour.z });
		glm::vec2 lineOfShoot = barrelOffset2D + fireDirection * 80.0f * glm::clamp(0.0f, chargedDuration / sniperChargeTime, 1.0f);
		lines->DrawLineSegment(glm::vec3(barrelOffset2D.x, barrelOffset.y, barrelOffset2D.y), glm::vec3(lineOfShoot.x, barrelOffset.y, lineOfShoot.y), {sniperBeamColour.x, sniperBeamColour.y, sniperBeamColour.z});
		lines->DrawCircle(glm::vec3(pos2D.x, 0.2f, pos2D.y), 100.0f * glm::clamp(0.0f, (chargedDuration - sniperChargeTime) / (overclockChargeTime - sniperChargeTime), 1.0f), { overclockBeamColour.x, overclockBeamColour.y, overclockBeamColour.z });
		// TODO: Max charge sound
	}
	else if (chargingShot)
	{
		chargingShot = false;
		playedReachCharge1 = false;
		playedReachCharge2 = false;


		if (chargedDuration >= overclockChargeTime)
		{
			ShootOverClocked(globalBarrelOffset);
			SceneManager::scene->audio.PlaySound(Audio::railgunShotFirstCharged);
		}
		else if (chargedDuration >= sniperChargeTime)
		{
			ShootSniper(globalBarrelOffset);
			SceneManager::scene->audio.PlaySound(Audio::railgunShotSecondCharged);
		}
		else
		{
			//DO NOT FIRING STUFF
		}
		chargedDuration = 0;
	}

	for (auto i = blasts.begin(); i != blasts.end();)
	{
		i->timeElapsed += delta;
		float r = 1.5f * i->colour.x - i->colour.x * i->timeElapsed / i->lifeSpan;
		float g = 1.5f * i->colour.y - i->colour.y * i->timeElapsed / i->lifeSpan;
		float b = 1.5f * i->colour.z - i->colour.z * i->timeElapsed / i->lifeSpan;
		lines->DrawLineSegment(i->startPosition, i->endPosition, {r,g,b});
		if (i->timeElapsed > i->lifeSpan)
			i = blasts.erase(i);
		else
			++i;
	}
	return timeSinceHealButtonPressed <= windowOfTimeForHealPressed;
}

void Sync::GUI()
{
	std::string tag = Utilities::PointerToString(this);
	if (!ImGui::CollapsingHeader(("Sync Component##" + tag).c_str()))
	{
		return;
	}
	ImGui::Indent();
	ImGui::DragFloat(("Move Speed##" + tag).c_str(), &moveSpeed);
	ImGui::DragFloat(("Max Move Force##" + tag).c_str(), &maxMoveForce, 1.0f, 0.0f, FLT_MAX);
	ImGui::DragFloat(("Stationary Stopping Force##" + tag).c_str(), &maxStopForce, 1.0f, 0.0f, FLT_MAX);
	ImGui::DragFloat(("Look DeadZone##" + tag).c_str(), &lookDeadZone);
	ImGui::DragFloat(("Move DeadZone##" + tag).c_str(), &moveDeadZone);
	if (ImGui::DragFloat(("Heal Button Tolerance##" + tag).c_str(), &windowOfTimeForHealPressed))
	{
		SceneManager::scene->ecco->windowOfTimeForHealPressed = windowOfTimeForHealPressed;
	}

	ImGui::DragFloat3(("Barrel Offset##" + tag).c_str(), &barrelOffset[0]);

	if (ImGui::CollapsingHeader(("Misfire Properties##" + tag).c_str()))
	{
		ImGui::DragInt(("Misfire Damage##" + tag).c_str(), &misfireDamage);
		ImGui::DragFloat(("Misfire Shot Speed##" + tag).c_str(), &misfireShotSpeed);
	}
	if (ImGui::CollapsingHeader(("Sniper Shot Properties##" + tag).c_str()))
	{
		ImGui::DragInt(("Sniper Damage##" + tag).c_str(), &sniperDamage);
		ImGui::DragFloat(("Sniper Charge Time##" + tag).c_str(), &sniperChargeTime);
		ImGui::DragFloat(("Sniper Beam life span##" + tag).c_str(), &sniperBeamLifeSpan);
		ImGui::ColorEdit3(("Sniper Beam Colour##" + tag).c_str(), &sniperBeamColour[0]);
	}
	if (ImGui::CollapsingHeader(("Overclock Shot Properties##" + tag).c_str()))
	{
		ImGui::DragInt(("Damage##" + tag).c_str(), &overclockDamage);
		ImGui::DragFloat(("Charge Time##" + tag).c_str(), &overclockChargeTime);
		ImGui::DragFloat(("Beam life span##" + tag).c_str(), &overclockBeamLifeSpan);
		ImGui::ColorEdit3(("Beam Colour##" + tag).c_str(), &overclockBeamColour[0]);
		ImGui::DragInt(("Max Enemy Pierce Count##" + tag).c_str(), &enemyPierceCount);
		ImGui::DragInt(("Rebound Count##" + tag).c_str(), &overclockReboundCount);
		ImGui::DragInt(("Refraction Beams Off Ecco##" + tag).c_str(), &eccoRefractionCount);
		ImGui::DragFloat(("Refraction Beams Angle##" + tag).c_str(), &eccoRefractionAngle);
	}
	if (ImGui::CollapsingHeader(("Game UI##" + tag).c_str())) {
		ImGui::SeparatorText(("Health##" + tag).c_str());
		ImGui::SliderFloat(("healthOffsetX##" + tag).c_str(), &healthOffsetX, -1.0f, 1.0f);
		ImGui::SliderFloat(("healthOffsetY##" + tag).c_str(), &healthOffsetY, -1.0f, 1.0f);
		ImGui::SliderFloat(("healthScaleX##" + tag).c_str(), &healthScaleX, 0.0f, 1.0f);
		ImGui::SliderFloat(("healthScaleY##" + tag).c_str(), &healthScaleY, 0.0f, 1.0f);
		ImGui::ColorEdit3(("health background##" + tag).c_str(), &healthBackgroundColour.x);
		ImGui::ColorEdit3(("health foreground##" + tag).c_str(), &healthForegroundColour.x);
		ImGui::SeparatorText(("Charge##" + tag).c_str());
		ImGui::SliderFloat(("chargeOffsetX##" + tag).c_str(), &chargeOffsetX, -1.0f, 1.0f);
		ImGui::SliderFloat(("chargeOffsetY##" + tag).c_str(), &chargeOffsetY, -1.0f, 1.0f);
		ImGui::SliderFloat(("chargeScaleX##" + tag).c_str(), &chargeScaleX, 0.0f, 1.0f);
		ImGui::SliderFloat(("chargeScaleY##" + tag).c_str(), &chargeScaleY, 0.0f, 1.0f);
		ImGui::ColorEdit3(("charge background##" + tag).c_str(), &chargeBackgroundColour.x);
		ImGui::ColorEdit3(("charge foreground##" + tag).c_str(), &chargeForegroundColour.x);
	}


	ImGui::Unindent();
}

toml::table Sync::Serialise() const
{
	return toml::table{
		{ "guid", Serialisation::SaveAsUnsignedLongLong(GUID)},
		{ "moveSpeed", moveSpeed },
		{ "lookDeadZone", lookDeadZone },
		{ "moveDeadZone", moveDeadZone },
		{ "barrelOffset", Serialisation::SaveAsVec3(barrelOffset) },
		{ "misfireDamage", misfireDamage },
		{ "misfireShotSpeed", misfireShotSpeed },
		{ "sniperDamage", sniperDamage },
		{ "sniperChargeTime", sniperChargeTime },
		{ "sniperBeamLifeSpan", sniperBeamLifeSpan },
		{ "sniperBeamColour", Serialisation::SaveAsVec3(sniperBeamColour) },
		{ "overclockDamage", overclockDamage },
		{ "overclockChargeTime", overclockChargeTime },
		{ "overclockBeamLifeSpan", overclockBeamLifeSpan },
		{ "overclockBeamColour", Serialisation::SaveAsVec3(overclockBeamColour) },
		{ "overclockReboundCount", overclockReboundCount },
		{ "enemyPierceCount", enemyPierceCount },
		{ "eccoRefractionAngle", eccoRefractionAngle },
		{ "eccoRefractionCount", eccoRefractionCount },
		{ "misfireColliderRadius", misfireColliderRadius },
		{ "maxMoveForce", maxMoveForce },
		{ "maxStopForce", maxStopForce },
		{ "healthOffsetX", healthOffsetX},
		{ "healthOffsetY", healthOffsetY},
		{ "healthScaleX", healthScaleX},
		{ "healthScaleY", healthScaleY},
		{ "chargeOffsetX", chargeOffsetX},
		{ "chargeOffsetY", chargeOffsetY},
		{ "chargeScaleX", chargeScaleX},
		{ "chargeScaleY", chargeScaleY},
		{ "healthBackgroundColour", Serialisation::SaveAsVec3(healthBackgroundColour) },
		{ "healthForegroundColour", Serialisation::SaveAsVec3(healthForegroundColour) },
		{ "chargeBackgroundColour", Serialisation::SaveAsVec3(chargeBackgroundColour) },
		{ "chargeForegroundColour", Serialisation::SaveAsVec3(chargeForegroundColour) },
	};
}

void Sync::ShootMisfire(glm::vec3 pos)
{
	SceneObject* shot = new SceneObject(SceneManager::scene);
	shot->setRenderer(misfireModelRender);

	shot->setRigidBody(new RigidBody(0.0f,0.0f));
	PolygonCollider* collider = new PolygonCollider({ {0.0f,0.0f} }, misfireColliderRadius, CollisionLayers::sync);
	collider->isTrigger = true;
	shot->rigidbody()->addCollider(collider);
	shot->rigidbody()->vel += fireDirection * misfireShotSpeed;
	shot->rigidbody()->onTrigger.push_back([this](Collision collision) {misfireShotOnCollision(collision); });
	shot->transform()->setPosition(pos);
	shot->transform()->setScale(0.1f);

	SceneManager::scene->audio.PlaySound(Audio::railgunMisfire);
}

void Sync::ShootSniper(glm::vec3 pos)
{
	std::vector<Hit> hits;
	if (PhysicsSystem::RayCast({ pos.x, pos.z }, fireDirection, hits, FLT_MAX, ~((int)CollisionLayers::sync | (int)CollisionLayers::ecco | Collider::transparentLayers)))
	{
		Hit hit = hits[0];
		blasts.push_back({ sniperBeamLifeSpan, 0.0f, sniperBeamColour, pos, {hit.position.x, pos.y, hit.position.y} });
		if (hit.collider->collisionLayer & (int)CollisionLayers::enemy)
		{
			hit.sceneObject->health()->subtractHealth(sniperDamage);
			SceneManager::scene->audio.PlaySound(Audio::enemyHitByShot);
		}
	}

	SceneManager::scene->audio.PlaySound(Audio::railgunShotFirstCharged);
}

void Sync::ShootOverClocked(glm::vec3 pos)
{
	OverclockRebounding(pos, fireDirection, 0, overclockBeamColour);
	SceneManager::scene->audio.PlaySound(Audio::railgunShotSecondCharged);
}

void Sync::OverclockRebounding(glm::vec3 pos, glm::vec2 dir, int count, glm::vec3 colour)
{
	std::vector<Hit> hits;
	if (PhysicsSystem::RayCast({ pos.x, pos.z }, dir, hits, FLT_MAX, ~((int)CollisionLayers::ecco | (int)CollisionLayers::ecco | Collider::transparentLayers)))
	{
		Hit hit = hits[0];
		if (hit.collider->collisionLayer & (int)CollisionLayers::enemy)
		{
			hit.sceneObject->health()->subtractHealth(overclockDamage);
			for (int i = 0; i < hits.size() && i < enemyPierceCount; i++)
			{
				hit = hits[i];
				if (i == enemyPierceCount - 1) return;
				if (hit.collider->collisionLayer & (int)CollisionLayers::enemy)
				{
					hit.sceneObject->health()->subtractHealth(overclockDamage);
					SceneManager::scene->audio.PlaySound(Audio::enemyHitByShot);
				}
				else break;
			}
		}

		blasts.push_back({ overclockBeamLifeSpan, 0.0f, colour, pos, {hit.position.x, pos.y, hit.position.y} });

		if (hit.collider->collisionLayer & (int)CollisionLayers::reflectiveSurface)
		{
			float s = 0.95f;
			float v = 0.95f;
			float angle = - eccoRefractionAngle / 2.0f;
			for (int iter = 0; iter < eccoRefractionCount; iter++, angle += eccoRefractionAngle / (eccoRefractionCount - 1))
			{
				float h = iter / (float)eccoRefractionCount;
				int i = int(h * 6.0f);
				float f = h * 6.0f - i;
				float w = v * (1.0f - s);
				float q = v * (1.0f - s * f);
				float t = v * (1.0f - s * (1.0f - f));

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

				SceneManager::scene->audio.PlaySound(SceneManager::scene->audio.rainbowShot);
			}
		}
		else if (count < overclockReboundCount && !(hit.collider->collisionLayer & (int)CollisionLayers::sync))
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
	PhysicsSystem::RayCast({ pos.x, pos.z }, dir, hits, FLT_MAX, ~((int)CollisionLayers::ecco | (int)CollisionLayers::reflectiveSurface | Collider::transparentLayers));
	Hit hit = hits[0];

	if(hit.collider->collisionLayer & (int)CollisionLayers::enemy)
	{
		hit.sceneObject->health()->subtractHealth(overclockDamage);
		SceneManager::scene->audio.PlaySound(Audio::enemyHitByShot);
		for (int i = 1; i < hits.size() && i < enemyPierceCount; i++)
		{
			hit = hits[i];
			if (hit.collider->collisionLayer & (int)CollisionLayers::enemy)
			{
				hit.sceneObject->health()->subtractHealth(overclockDamage);
				SceneManager::scene->audio.PlaySound(Audio::enemyHitByShot);
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
	//TODO DELETE IN SCENE
	unsigned long long GUID = collision.self->GUID;
	delete collision.self;
	SceneManager::scene->sceneObjects.erase(GUID);

	SceneManager::scene->audio.PlaySound(Audio::railgunHitEnemy);
}

