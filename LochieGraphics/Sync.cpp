#include "Sync.h"

#include "Transform.h"
#include "RigidBody.h"
#include "SceneObject.h"
#include "Health.h"
#include "ModelRenderer.h"
#include "LineRenderer.h"
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
#include "VelocityCondition.h"
#include "SyncChargingCondition.h"
#include "SyncChargeLevelCondition.h"
#include "AnimateBehaviour.h"
#include "State.h"
#include "AnimationFinishedCondition.h"
#include "BlastLine.h"

#include "EditorGUI.h"

#include "Serialisation.h"

Sync::Sync()
{
}

Sync::Sync(toml::table table) : Sync()
{
	Load(table);
}

void Sync::Load(toml::table table)
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
	healthUI.Load(table["healthUI"].as_table());
	chargeUI.Load(table["chargeUI"].as_table());
	startSlowTime = Serialisation::LoadAsFloat(table["startSlowTime"], 0.5f);
	stopSlowTime = Serialisation::LoadAsFloat(table["stopSlowTime"], 1.5f);
	knockBackForceOverclock = Serialisation::LoadAsFloat(table["knockBackForceOverclock"], 1.0f);
	knockBackForceSnipe = Serialisation::LoadAsFloat(table["knockBackForceSnipe"], 10.0f);
	rainbowDimming = Serialisation::LoadAsFloat(table["rainbowDimming"], 1.0f);
	overclockBounceDamage = Serialisation::LoadAsInt(table["overclockBounceDamage"]);
	rainbowRebounding = Serialisation::LoadAsBool(table["rainbowRebounding"]);
	shotWidth = Serialisation::LoadAsFloat(table["shotWidth"], 10.0f);

	stateMachineSetup = false;
}

void Sync::Start(SceneObject* sceneObjectWithAnimator)
{
	Model* misfireModel = ResourceManager::LoadModelAsset(Paths::modelSaveLocation + misfireModelPath + Paths::modelExtension);
	Material* misfireMaterial = ResourceManager::defaultMaterial;
	misfireModelRender = new ModelRenderer(misfireModel, misfireMaterial);



	VelocityCondition* movingEnoughToRun = new VelocityCondition(1.0f, FLT_MAX);
	animatorStateMachine.AddCondition(movingEnoughToRun);
	VelocityCondition* standingStill = new VelocityCondition(0.0, 1.0f);
	animatorStateMachine.AddCondition(standingStill);
	SyncChargingCondition* charging = new SyncChargingCondition();
	animatorStateMachine.AddCondition(charging);
	SyncChargingCondition* notCharging = new SyncChargingCondition(true);
	animatorStateMachine.AddCondition(notCharging);
	SyncChargeLevelCondition* noCharge = new SyncChargeLevelCondition(ChargeLevel::none, false);
	animatorStateMachine.AddCondition(noCharge);
	SyncChargeLevelCondition* snipeCharge = new SyncChargeLevelCondition(ChargeLevel::sniper, false);
	animatorStateMachine.AddCondition(snipeCharge);
	SyncChargeLevelCondition* overclockCharge = new SyncChargeLevelCondition(ChargeLevel::overclock, false);
	animatorStateMachine.AddCondition(overclockCharge);
	AnimationFinishedCondition* animationFinished = new AnimationFinishedCondition(AnimationFinishedCondition::Check::current);

	State* idle = new State(new AnimateBehaviour(ResourceManager::LoadAnimationAsset(Paths::animationsSaveLocation + "SyncIdle" + Paths::animationExtension), true));
	animatorStateMachine.AddState(idle);
	State* run = new State(new AnimateBehaviour(ResourceManager::LoadAnimationAsset(Paths::animationsSaveLocation + "SyncRun" + Paths::animationExtension), true));
	animatorStateMachine.AddState(run);
	State* shootCharge = new State(new AnimateBehaviour(ResourceManager::LoadAnimationAsset(Paths::animationsSaveLocation + "SyncShootCharge" + Paths::animationExtension), false));
	animatorStateMachine.AddState(shootCharge);
	State* shootHold = new State(new AnimateBehaviour(ResourceManager::LoadAnimationAsset(Paths::animationsSaveLocation + "SyncShootHold" + Paths::animationExtension), true));
	animatorStateMachine.AddState(shootHold);
	State* shootSnipe = new State(new AnimateBehaviour(ResourceManager::LoadAnimationAsset(Paths::animationsSaveLocation + "SyncShootSnipe" + Paths::animationExtension), false));
	animatorStateMachine.AddState(shootSnipe);
	State* shootOverclock = new State(new AnimateBehaviour(ResourceManager::LoadAnimationAsset(Paths::animationsSaveLocation + "SyncShootOverclock" + Paths::animationExtension), false));
	animatorStateMachine.AddState(shootOverclock);

	State* start = idle;

	idle->AddTransition(movingEnoughToRun, run);
	idle->AddTransition(charging, shootCharge);
	run->AddTransition(standingStill, start);
	run->AddTransition(charging, shootCharge);
	shootCharge->AddTransition(animationFinished, shootHold);
	shootCharge->AddTransition(noCharge, start);
	shootCharge->AddTransition(snipeCharge, shootSnipe);
	shootCharge->AddTransition(overclockCharge, shootOverclock);
	shootHold->AddTransition(snipeCharge, shootSnipe);
	shootHold->AddTransition(overclockCharge, shootOverclock);
	shootSnipe->AddTransition(animationFinished, start);
	shootOverclock->AddTransition(animationFinished, start);

	// TODO: Death animations

	animatorStateMachine.setInitialState(start);
	if (sceneObjectWithAnimator) {
		animatorStateMachine.Enter(sceneObjectWithAnimator);
	}
	stateMachineSetup = true;
}

bool Sync::Update(
	SceneObject* sceneObjectWithAnimator, Input::InputDevice& inputDevice, Transform& transform,
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
		float currentMoveSpeed = moveSpeed;
		if (chargedDuration >= startSlowTime)
		{
			currentMoveSpeed = Utilities::Lerp(moveSpeed, 0.0f, (glm::min(chargedDuration, stopSlowTime) - startSlowTime) / (stopSlowTime - startSlowTime));
		}
		
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
				if (!reachedCharge1) {
					reachedCharge1 = true;
					SceneManager::scene->audio.PlaySound(Audio::railgunFirstChargeReached);
				}
			}
			else if (chargedDuration + delta >= overclockChargeTime && chargedDuration < overclockChargeTime)
			{
				//At this time the charge is enough to shoot the reflecting shot
				if (!reachedCharge2) {
					reachedCharge2 = true;
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
		reachedCharge1 = false;
		reachedCharge2 = false;


		if (chargedDuration >= overclockChargeTime)
		{
			ShootOverClocked(globalBarrelOffset);
			rigidBody.AddImpulse(-fireDirection * knockBackForceOverclock);
			SceneManager::scene->audio.PlaySound(Audio::railgunShotFirstCharged);
			lastShotLevel = ChargeLevel::overclock;
		}
		else if (chargedDuration >= sniperChargeTime)
		{
			ShootSniper(globalBarrelOffset);
			rigidBody.AddImpulse(-fireDirection * knockBackForceSnipe);
			SceneManager::scene->audio.PlaySound(Audio::railgunShotSecondCharged);
			lastShotLevel = ChargeLevel::sniper;
		}
		else
		{
			lastShotLevel = ChargeLevel::none;
			//DO NOT FIRING STUFF
		}
		chargedDuration = 0;
	}

	if (sceneObjectWithAnimator) {
		if (!stateMachineSetup) {
			animatorStateMachine.setInitialState(animatorStateMachine.getInitialState());
			animatorStateMachine.Enter(sceneObjectWithAnimator);
			stateMachineSetup = true;
		}
		animatorStateMachine.Update(sceneObjectWithAnimator, delta);
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
	ImGui::DragFloat(("Start Slow Time##" + tag).c_str(), &startSlowTime, 0.02f, 0, stopSlowTime);
	ImGui::DragFloat(("Stop Slow Time##" + tag).c_str(), &stopSlowTime, 0.02f, startSlowTime);
	if (ImGui::DragFloat(("Heal Button Tolerance##" + tag).c_str(), &windowOfTimeForHealPressed))
	{
		SceneManager::scene->ecco->windowOfTimeForHealPressed = windowOfTimeForHealPressed;
	}

	ImGui::DragFloat3(("Barrel Offset##" + tag).c_str(), &barrelOffset[0]);
	ImGui::DragFloat(("Shot Width##" + tag).c_str(), &shotWidth);
	ImGui::DragInt(("Max Enemy Pierce Count##" + tag).c_str(), &enemyPierceCount);

	if (ImGui::CollapsingHeader(("Sniper Shot Properties##" + tag).c_str()))
	{
		ImGui::DragInt(("Sniper Damage##" + tag).c_str(), &sniperDamage);
		ImGui::DragInt(("Sniper Rebound Count##" + tag).c_str(), &sniperReboundCount);
		ImGui::DragFloat(("Sniper Charge Time##" + tag).c_str(), &sniperChargeTime);
		ImGui::DragFloat(("Sniper Beam life span##" + tag).c_str(), &sniperBeamLifeSpan);
		ImGui::ColorEdit3(("Sniper Beam Colour##" + tag).c_str(), &sniperBeamColour[0]);
		ImGui::DragFloat(("Knock Back Force Snipe##" + tag).c_str(), &knockBackForceSnipe);
	}
	if (ImGui::CollapsingHeader(("Overclock Shot Properties##" + tag).c_str()))
	{
		ImGui::DragInt(("Damage##" + tag).c_str(), &overclockDamage);
		ImGui::DragFloat(("Charge Time##" + tag).c_str(), &overclockChargeTime);
		ImGui::DragFloat(("Beam life span##" + tag).c_str(), &overclockBeamLifeSpan);
		ImGui::ColorEdit3(("Beam Colour##" + tag).c_str(), &overclockBeamColour[0]);
		ImGui::DragFloat(("Knock Back Force Overclock##" + tag).c_str(), &knockBackForceOverclock);
		ImGui::DragInt(("OverClock Rebound Count##" + tag).c_str(), &overclockReboundCount);
		ImGui::DragInt(("Refraction Beams Off Ecco##" + tag).c_str(), &eccoRefractionCount);
		ImGui::DragFloat(("Refraction Beams Angle##" + tag).c_str(), &eccoRefractionAngle);
		ImGui::Checkbox(("Rainbow Shots Rebound##" + tag).c_str(), &rainbowRebounding);
		ImGui::DragFloat(("Rainbow Rebound Dimming##" + tag).c_str(), &rainbowDimming);
		ImGui::DragInt(("Rainbow Rebound Damage##" + tag).c_str(), &overclockBounceDamage);
	}
	if (ImGui::CollapsingHeader(("Health UI##" + tag).c_str())) {
		healthUI.GUI();
	}
	if (ImGui::CollapsingHeader(("Charge UI##" + tag).c_str())) {
		chargeUI.GUI();
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
		{ "sniperReboundCount", sniperReboundCount },
		{ "enemyPierceCount", enemyPierceCount },
		{ "eccoRefractionAngle", eccoRefractionAngle },
		{ "eccoRefractionCount", eccoRefractionCount },
		{ "misfireColliderRadius", misfireColliderRadius },
		{ "maxMoveForce", maxMoveForce },
		{ "maxStopForce", maxStopForce },
		{ "healthUI", healthUI.Serialise() },
		{ "chargeUI", chargeUI.Serialise() },
		{ "startSlowTime", startSlowTime },
		{ "stopSlowTime", stopSlowTime },
		{ "rainbowDimming", rainbowDimming },
		{ "rainbowRebounding", rainbowRebounding },
		{ "overclockBounceDamage", overclockBounceDamage },
		{ "knockBackForceOverclock", knockBackForceOverclock },
		{ "knockBackForceSnipe", knockBackForceSnipe },
		{ "shotWidth", shotWidth},
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
	if (PhysicsSystem::RayCastRadiusExpansion({ pos.x, pos.z }, fireDirection, hits, shotWidth, FLT_MAX, ~((int)CollisionLayers::sync | (int)CollisionLayers::ecco | Collider::transparentLayers)))
	{
		Hit hit = hits[0];
		glm::vec3 pos3D = { hit.position.x, pos.y, hit.position.y };
		Particle* hitFX = SceneManager::scene->particleSystem.AddParticle(100, 0.35f, SceneManager::scene->particleSystem.nextParticleTexture, pos3D);
		hitFX->explodeStrength = 3.0f;
		hitFX->Explode();
		RenderSystem::beams.push_back({ sniperBeamLifeSpan, 0.0f, sniperBeamColour, pos, {hit.position.x, pos.y, hit.position.y} });
		if (hit.collider->collisionLayer & (int)CollisionLayers::enemy)
		{
			hit.sceneObject->health()->subtractHealth(sniperDamage);
			SceneManager::scene->audio.PlaySound(Audio::enemyHitByShot);
			for (int i = 0; i < hits.size() && i < enemyPierceCount; i++)
			{
				hit = hits[i];
				if (i == enemyPierceCount - 1) return;

				glm::vec3 pos3D = { hit.position.x, pos.y, hit.position.y };
				Particle* hitFX = SceneManager::scene->particleSystem.AddParticle(100, 0.35f, SceneManager::scene->particleSystem.nextParticleTexture, pos3D);
				hitFX->explodeStrength = 3.0f;
				hitFX->Explode();
				if (hit.collider->collisionLayer & (int)CollisionLayers::enemy)
				{
					hit.sceneObject->health()->subtractHealth(sniperDamage);
					SceneManager::scene->audio.PlaySound(Audio::enemyHitByShot);
				}
				else break;
			}
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
	if (PhysicsSystem::RayCastRadiusExpansion({ pos.x, pos.z }, dir, hits, shotWidth, FLT_MAX, ~((int)CollisionLayers::ecco | (int)CollisionLayers::ecco | Collider::transparentLayers)))
	{
		Hit hit = hits[0];
		glm::vec3 pos3D = { hit.position.x, pos.y, hit.position.y };
		Particle* hitFX = SceneManager::scene->particleSystem.AddParticle(100, 0.35f, SceneManager::scene->particleSystem.nextParticleTexture, pos3D);
		hitFX->explodeStrength = 3.0f;
		hitFX->Explode();
		if (hit.collider->collisionLayer & (int)CollisionLayers::enemy)
		{
			hit.sceneObject->health()->subtractHealth(overclockDamage);
			for (int i = 0; i < hits.size() && i < enemyPierceCount; i++)
			{
				hit = hits[i];
				if (i == enemyPierceCount - 1) return;

				glm::vec3 pos3D = { hit.position.x, pos.y, hit.position.y };
				Particle* hitFX = SceneManager::scene->particleSystem.AddParticle(100, 0.35f, SceneManager::scene->particleSystem.nextParticleTexture, pos3D);
				hitFX->explodeStrength = 3.0f;
				hitFX->Explode();
				if (hit.collider->collisionLayer & (int)CollisionLayers::enemy)
				{
					hit.sceneObject->health()->subtractHealth(overclockDamage);
					SceneManager::scene->audio.PlaySound(Audio::enemyHitByShot);
				}
				else break;
			}
		}

		RenderSystem::beams.push_back({ overclockBeamLifeSpan, 0.0f, colour, pos, {hit.position.x, pos.y, hit.position.y} });

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

				OverclockRaindowShot({ hit.position.x, pos.y, hit.position.y }, refractionDirection, refractionColour, rainbowRebounding);

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

void Sync::OverclockRaindowShot(glm::vec3 pos, glm::vec2 dir, glm::vec3 colour, bool rebound)
{
	std::vector<Hit> hits;
	if (PhysicsSystem::RayCastRadiusExpansion({ pos.x, pos.z }, dir, hits, shotWidth, FLT_MAX, ~((int)CollisionLayers::ecco | (int)CollisionLayers::reflectiveSurface | Collider::transparentLayers)))
	{
		Hit hit = hits[0];

		glm::vec3 pos3D = { hit.position.x, pos.y, hit.position.y };
		Particle* hitFX = SceneManager::scene->particleSystem.AddParticle(100, 0.35f, SceneManager::scene->particleSystem.nextParticleTexture, pos3D);
		hitFX->explodeStrength = 3.0f;
		hitFX->Explode();

		if (hit.collider->collisionLayer & (int)CollisionLayers::enemy)
		{
			hit.sceneObject->health()->subtractHealth(overclockDamage);
			SceneManager::scene->audio.PlaySound(Audio::enemyHitByShot);
			for (int i = 1; i < hits.size() && i < enemyPierceCount; i++)
			{
				hit = hits[i];

				glm::vec3 pos3D = { hit.position.x, pos.y, hit.position.y };
				Particle* hitFX = SceneManager::scene->particleSystem.AddParticle(100, 0.35f, SceneManager::scene->particleSystem.nextParticleTexture, pos3D);
				hitFX->explodeStrength = 3.0f;
				hitFX->Explode();

				if (hit.collider->collisionLayer & (int)CollisionLayers::enemy)
				{
					hit.sceneObject->health()->subtractHealth(rebound ? overclockDamage : overclockBounceDamage);
					SceneManager::scene->audio.PlaySound(Audio::enemyHitByShot);
				}
				else break;
			}
		}
		else if (rebound)
		{
			OverclockRaindowShot({ hit.position.x,pos.y, hit.position.y }, glm::reflect(dir, hit.normal), colour / rainbowDimming, false);
		}

		RenderSystem::beams.push_back({ overclockBeamLifeSpan, 0.0f, colour, pos, {hit.position.x, pos.y, hit.position.y} });
	}
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

void Sync::LevelLoad()
{
}

