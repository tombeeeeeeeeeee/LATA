#pragma once
#include <unordered_map>
class Transform;
class Door;
class Bollard;

class DoorAndBollardSystem
{
public :
	DoorAndBollardSystem() {};
	//TODO:
	/*
	* Move Doors and Bollards to correct state
	*/

	void Start(
		std::unordered_map<unsigned long long, Transform>& transforms,
		std::unordered_map<unsigned long long, Door>& doors
	);

	void Update(
		std::unordered_map<unsigned long long, Transform>& transforms,
		std::unordered_map<unsigned long long, Door>& doors,
		float delta
	);

};