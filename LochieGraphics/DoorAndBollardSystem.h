#pragma once
#include <unordered_map>
class Transform;
class Door;
class Bollard;
struct Collider;

class DoorAndBollardSystem
{
public :
	DoorAndBollardSystem() {};
	//TODO:
	/*
	* Move Doors and Bollards to correct state
	*/

	float HeightToLower = 100.0f;

	void Start(
		std::unordered_map<unsigned long long, Transform>& transforms,
		std::unordered_map<unsigned long long, Door>& doors
	);

	void Update(
		std::unordered_map<unsigned long long, Transform>& transforms,
		std::unordered_map<unsigned long long, Door>& doors,
		std::unordered_map<unsigned long long, Bollard>& bollards,
		std::unordered_map<unsigned long long, Collider*>& colliders,
		float delta
	);

};