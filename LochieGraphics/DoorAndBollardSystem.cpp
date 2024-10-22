#include "DoorAndBollardSystem.h"
#include "Door.h"
#include "Bollard.h"
#include "Collider.h"
#include "Transform.h"
#include "Utilities.h"

void DoorAndBollardSystem::Start(
	std::unordered_map<unsigned long long, Transform>& transforms,
	std::unordered_map<unsigned long long, Door>& doors
)
{
	for (auto& doorPair : doors)
	{
		doorPair.second.pos = transforms[doorPair.first].getPosition();
	}
}

void DoorAndBollardSystem::Update(
	std::unordered_map<unsigned long long, Transform>& transforms,
	std::unordered_map<unsigned long long, Door>& doors,
	std::unordered_map<unsigned long long, Bollard>& bollards,
	std::unordered_map<unsigned long long, Collider*>& colliders,
	float delta
)
{
	for (auto& doorPair : doors)
	{
		int flip = doorPair.second.left ? -1 : 1;
		float moveAmount = doorPair.second.timeInMovement;
		if (doorPair.second.startClosed && doorPair.second.state)
		{
			doorPair.second.timeInMovement = glm::clamp(doorPair.second.timeInMovement, 0.0f, doorPair.second.timeToOpen);
			moveAmount /= doorPair.second.timeToOpen;
			doorPair.second.timeInMovement += delta;
			glm::clamp(doorPair.second.timeInMovement, 0.0f, doorPair.second.timeToOpen);
		}
		else if (doorPair.second.startClosed)
		{
			doorPair.second.timeInMovement = glm::clamp(doorPair.second.timeInMovement, 0.0f, doorPair.second.timeToClose);
			moveAmount /= doorPair.second.timeToClose;
			doorPair.second.timeInMovement -= delta;
			glm::clamp(doorPair.second.timeInMovement, 0.0f, doorPair.second.timeToClose);
		}
		else if (doorPair.second.state)
		{
			doorPair.second.timeInMovement = glm::clamp(doorPair.second.timeInMovement, 0.0f, doorPair.second.timeToClose);
			moveAmount /= doorPair.second.timeToClose;			
			doorPair.second.timeInMovement += delta;
			glm::clamp(doorPair.second.timeInMovement, 0.0f, doorPair.second.timeToClose);
		}
		else
		{
			doorPair.second.timeInMovement = glm::clamp(doorPair.second.timeInMovement, 0.0f, doorPair.second.timeToOpen);
			moveAmount /= doorPair.second.timeToOpen;
			doorPair.second.timeInMovement -= delta;
			glm::clamp(doorPair.second.timeInMovement, 0.0f, doorPair.second.timeToOpen);
		}
		moveAmount = glm::clamp(moveAmount, 0.0f, 1.0f) * flip;
		moveAmount *= doorPair.second.amountToMove;
		transforms[doorPair.first].setPosition(
			doorPair.second.pos + transforms[doorPair.first].forward() * moveAmount
		);
	}
	for (auto& bolPair : bollards)
	{
		float height = 0;

		float influence = bolPair.second.timeInProcess;
		if (bolPair.second.state && bolPair.second.startsUp)
		{
			bolPair.second.timeInProcess = glm::clamp(bolPair.second.timeInProcess, 0.0f, bolPair.second.timeToLower);
			influence /= bolPair.second.timeToLower;
			bolPair.second.timeInProcess += delta;
		}
		else if (bolPair.second.state)
		{
			bolPair.second.timeInProcess = glm::clamp(bolPair.second.timeInProcess, 0.0f, bolPair.second.timeToRaise);
			influence /= bolPair.second.timeToRaise;
			bolPair.second.timeInProcess += delta;
		}
		else if (bolPair.second.startsUp)
		{
			bolPair.second.timeInProcess = glm::clamp(bolPair.second.timeInProcess, 0.0f, bolPair.second.timeToRaise);
			influence /= bolPair.second.timeToRaise;
			bolPair.second.timeInProcess -= delta;
		}
		else
		{
			bolPair.second.timeInProcess = glm::clamp(bolPair.second.timeInProcess, 0.0f, bolPair.second.timeToLower);
			influence /= bolPair.second.timeToLower;
			bolPair.second.timeInProcess -= delta;
		}
		influence = glm::clamp(influence, 0.0f, 1.0f);
		if (bolPair.second.startsUp)
		{
			influence = 1.0f - influence;
		}
			

		height = Utilities::Lerp(-HeightToLower, 0.0f, influence);
		if (influence > 0.8f)
		{
			colliders[bolPair.first]->collisionLayer = (int)CollisionLayers::softCover;
		}
		else if (influence > 0.45f)
		{
			colliders[bolPair.first]->collisionLayer = (int)CollisionLayers::halfCover;
		}
		else
		{
			colliders[bolPair.first]->collisionLayer = (int)CollisionLayers::count;
		}

		glm::vec3 p = transforms[bolPair.first].getPosition();
		transforms[bolPair.first].setPosition({ p.x, height, p.z });
	}
}
