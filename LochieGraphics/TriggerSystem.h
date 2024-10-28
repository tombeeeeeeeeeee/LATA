#pragma once
#include <map>
#include <string>
#include <unordered_map>

class SceneObject;
class RigidBody;
class Door;
class Plate;
class SpawnManager;
class PressurePlate;
class Bollard;
class Triggerable;

class TriggerSystem
{
public:
	static std::multimap<std::string, unsigned long long> triggerables;
	static void TriggerTag(std::string tag, bool toggle);

	void Start(
		std::unordered_map<unsigned long long, RigidBody>& rigidbodies,
		std::unordered_map<unsigned long long, PressurePlate>& plates,
		std::unordered_map<unsigned long long, SpawnManager>& spawnManagers,
		std::unordered_map<unsigned long long, Door>& doors,
		std::unordered_map<unsigned long long, Bollard>& bollards,
		std::unordered_map<unsigned long long, Triggerable>& triggerables
	);

	void Update(
		std::unordered_map<unsigned long long, PressurePlate>& plates,
		std::unordered_map<unsigned long long, Triggerable>& triggerables
	);

	void Clear();

	//TODO:
	/*
	multimap of triggerables

	methods:
		start - populate multimap with things who want to trigger, and triggers that want to listen
	*/

};

