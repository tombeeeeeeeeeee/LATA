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
		float moveAmount = flip * doorPair.second.timeInMovement;
		if (doorPair.second.startClosed && doorPair.second.state)
		{
			moveAmount /= doorPair.second.timeToOpen;
			doorPair.second.timeInMovement += delta;
			glm::clamp(doorPair.second.timeInMovement, 0.0f, doorPair.second.timeToOpen);
		}
		else if (doorPair.second.startClosed)
		{
			moveAmount /= doorPair.second.timeToClose;
			doorPair.second.timeInMovement -= delta;
			glm::clamp(doorPair.second.timeInMovement, 0.0f, doorPair.second.timeToClose);
		}
		else if (doorPair.second.state)
		{
			moveAmount /= doorPair.second.timeToClose;			
			doorPair.second.timeInMovement += delta;
			glm::clamp(doorPair.second.timeInMovement, 0.0f, doorPair.second.timeToClose);
		}
		else
		{
			moveAmount /= doorPair.second.timeToOpen;
			doorPair.second.timeInMovement -= delta;
			glm::clamp(doorPair.second.timeInMovement, 0.0f, doorPair.second.timeToOpen);
		}
		moveAmount = glm::clamp(moveAmount, 0.0f, 1.0f);
		moveAmount *= doorPair.second.amountToMove;
		transforms[doorPair.first].setPosition(
			doorPair.second.pos + transforms[doorPair.first].right() * moveAmount
		);
	}
	for (auto& bolPair : bollards)
	{
		float height = 0;

		float influence = bolPair.second.timeInProcess;
		if (bolPair.second.state && bolPair.second.startsUp)
		{
			influence /= bolPair.second.timeToLower;
			bolPair.second.timeInProcess += delta;
		}
		else if (bolPair.second.state)
		{
			influence /= bolPair.second.timeToRaise;
			bolPair.second.timeInProcess += delta;
		}
		else if (bolPair.second.startsUp)
		{
			influence /= bolPair.second.timeToRaise;
			bolPair.second.timeInProcess -= delta;
		}
		else
		{
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
			colliders[bolPair.first]->collisionLayer = (int)CollisionLayers::count;
		}
		else if (influence > 0.45f)
		{
			colliders[bolPair.first]->collisionLayer = (int)CollisionLayers::halfCover;
		}
		else
		{
			colliders[bolPair.first]->collisionLayer = (int)CollisionLayers::softCover;
		}

		glm::vec3 p = transforms[bolPair.first].getPosition();
		transforms[bolPair.first].setPosition({ p.x, height, p.z });
	}
}
