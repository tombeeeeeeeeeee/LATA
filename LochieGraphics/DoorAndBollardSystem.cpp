#include "DoorAndBollardSystem.h"
#include "Door.h"
#include "Bollard.h"
#include "Transform.h"

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
}
