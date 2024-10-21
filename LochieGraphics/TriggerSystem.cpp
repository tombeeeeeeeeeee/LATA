#include "TriggerSystem.h"
#include "SceneManager.h"
#include "Scene.h"
#include "SceneObject.h"
#include "SpawnManager.h"
#include "RigidBody.h"
#include "Collider.h"
#include "Collision.h"
#include "PressurePlate.h"
#include <iostream>

std::multimap<std::string, unsigned long long> TriggerSystem::triggerables = {};

void TriggerSystem::TriggerTag(std::string tag, bool toggle)
{
	auto range = triggerables.equal_range(tag);
	for (auto& it = range.first; it != range.second; it++)
	{
		SceneManager::scene->sceneObjects[(*it).second]->TriggerCall(tag, toggle);
	}
}

void TriggerSystem::Start(
	std::unordered_map<unsigned long long, RigidBody>& rigidbodies,
	std::unordered_map<unsigned long long, PressurePlate>& plates,
	std::unordered_map<unsigned long long, SpawnManager>& spawnManagers,
	std::unordered_map<unsigned long long, Door>& doors,
	std::unordered_map<unsigned long long, Bollard>& bollards
)
{
	for (auto& spawnPair : spawnManagers)
	{
		TriggerSystem::triggerables.insert({ spawnPair.second.triggerTag, spawnPair.first });
		if (SceneManager::scene->sceneObjects[spawnPair.first]->parts & Parts::rigidBody)
		{
			rigidbodies[spawnPair.first].onTrigger.push_back([this, spawnPair](Collision collision) 
				{if (collision.collisionMask & ((int)CollisionLayers::ecco | (int)CollisionLayers::sync)) TriggerSystem::TriggerTag(spawnPair.second.triggerTag, true); });
		}
	}
	for (auto& platePair : plates)
	{
		if (!(SceneManager::scene->sceneObjects[platePair.first]->parts & Parts::rigidBody))
		{
			SceneManager::scene->sceneObjects[platePair.first]->setRigidBody(new RigidBody());
		}

		rigidbodies[platePair.first].onTrigger.push_back(
			[platePair](Collision collision) { SceneManager::scene->plates[platePair.first].OnTrigger(collision.collisionMask); });
	}
	for (auto& pair : doors)
	{
		TriggerSystem::triggerables.insert({ pair.second.triggerTag, pair.first });
	}
	for (auto& pair : bollards)
	{
		TriggerSystem::triggerables.insert({ pair.second.triggerTag, pair.first });
	}
}

void TriggerSystem::Update(std::unordered_map<unsigned long long, PressurePlate>& plates)
{
	for (auto& platePair : plates)
	{
		if (!platePair.second.eccoToggled)
		{
			if (platePair.second.triggeredLastFrame && !platePair.second.triggeredThisFrame)
			{
				TriggerSystem::TriggerTag(platePair.second.triggerTag, false);
			}
			platePair.second.triggeredLastFrame = platePair.second.triggeredThisFrame;
			platePair.second.triggeredThisFrame = false;
		}
	}
}

void TriggerSystem::Clear()
{
	triggerables.clear();
}
