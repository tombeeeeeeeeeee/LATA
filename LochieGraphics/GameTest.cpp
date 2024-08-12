#include "GameTest.h"
#include "Utilities.h"
#include "ResourceManager.h"

#include "Image.h"

bool GameTest::MapCellIs(unsigned char* cell, unsigned char r, unsigned char g, unsigned char b)
{
	return cell[0] == r && cell[1] == g && cell[2] == b;
}

GameTest::GameTest()
{
}

void GameTest::Start()
{
	lights.insert(lights.end(), {
	&directionalLight,
	&spotlight,
	&pointLights[0],
	&pointLights[1],
	&pointLights[2],
	&pointLights[3],
	});
	
	hRb = new RigidBody();
	hRb->setMass(1.0f);
	hRb->setMomentOfInertia(5.0f);

	rRb = new RigidBody();
	rRb->setMass(0.1f);
	rRb->setMomentOfInertia(5.0f);

	input.Initialise();

	h->setRigidBody(hRb);
	r->setRigidBody(rRb);

	hRb = &rigidBodies[h->GUID];
	rRb = &rigidBodies[r->GUID];

	wheelDirection = {r->transform()->forward().x, r->transform()->forward().y};

	camera->pitch = -89.0f;
	camera->yaw = -90.0f;
	camera->position = { 0.0f, 1.5f, 0.0f };
	camera->farPlane = 1000.0f;
	camera->nearPlane = 0.1f;
	camera->UpdateVectors();

	level.path = "level.png";
	level.Load();

	SceneObject* newSceneObject = new SceneObject(this);
	PolygonCollider* newCollider = new PolygonCollider();
	newSceneObject->setCollider(newCollider);
	newCollider->verts = { 
		{  -halfGridSpacing, halfGridSpacing},
		{   halfGridSpacing, halfGridSpacing},
		{  halfGridSpacing, -halfGridSpacing},
		{ -halfGridSpacing, -halfGridSpacing},
	};
	newCollider->radius = halfGridSpacing;
	RigidBody* newRigidBody = new RigidBody(1.0f, 0.25f, { newCollider }, 0, true);
	newSceneObject->setRigidBody(newRigidBody);
}

void GameTest::Update(float delta)
{
	input.Update();

	LineRenderer& lines = renderSystem->lines;

	physicsSystem.UpdateRigidBodies(transforms, rigidBodies, delta);

	if (input.inputDevices.size() > 0) {

		/*
		* check if input is beyond the deadzone
		* dot the input with the transform.right()
		* desired wheel angle = clamp(asin(the dot product), -maxAngle, maxAngle);
		* desired wheel direction = transform.forward() rotated about desired wheel angle
		* wheel direction = lerp(wheelDirection, desiredWheeldirection, amountPerSec)
		* 
		* check if trigger is on
		* add force in direction of wheel direction
		* 
		* if veloctiy exceeds friction coefficent
		* spin (add rotational Impulse)
		* 
		*/
		
		Input::InputDevice* rC = input.inputDevices[0];

		glm::vec3 right = r->transform()->right();
		glm::vec3 forward = r->transform()->forward();

		glm::vec2 moveInput = rC->getMove();
		if (glm::length(moveInput) > deadZone)
		{
			moveInput = glm::normalize(moveInput);

			float turnAmount = glm::dot(glm::vec2(right.x, right.z), moveInput);
			turnAmount = glm::clamp(turnAmount, -1.0f, 1.0f);
			float desiredAngle = glm::asin(turnAmount);
			desiredAngle = glm::clamp(desiredAngle, -maxWheelAngle, maxWheelAngle);
			
			float c = cosf(desiredAngle);
			float s = sinf(desiredAngle);
			glm::vec2 desiredWheelDirection = {forward.x * c - forward.z * s, forward.z * c + forward.x * s };
			wheelDirection += (desiredWheelDirection - wheelDirection) * wheelTurnSpeed * delta;
			wheelDirection = glm::normalize(wheelDirection);
		}
		
		glm::vec2 force;
		if (glm::length(rC->getRightTrigger()) > 0.1f)
		{
			force = wheelDirection * carMoveSpeed * rC->getRightTrigger();
		}
		else
		{
			force = -rRb->vel * stoppingFrictionCoef;
		}

		force += -glm::abs(glm::dot(wheelDirection, rRb->vel)) * sidewaysFrictionCoef * rRb->vel;

		if (glm::length(rRb->vel + rRb->invMass * (force + rRb->netForce)) > maxCarMoveSpeed)
		{
			force = glm::normalize(force) * (maxCarMoveSpeed - glm::length(rRb->vel)) / rRb->invMass;
		}
		rRb->netForce += force;
		rRb->angularVel = -turningCircleScalar * glm::length(rRb->vel) * glm::dot({right.x, right.z}, wheelDirection);

		//TODO add skidding.
	}
	



	// Draw robot
	glm::vec3 rPos = r->transform()->getPosition();
	rot = r->transform()->getEulerRotation().y / 180 * PI;
	glm::vec3 rotary = r->transform()->getEulerRotation();
	std::vector<glm::vec3> robot =
	{
		{ 0.0f - hRadius, rPos.y, 0.0f - hRadius },
		{ 0.0f - hRadius, rPos.y, 0.0f + hRadius },
		{ 0.0f + hRadius, rPos.y, 0.0f + hRadius },
		{ 0.0f + hRadius, rPos.y, 0.0f - hRadius },
		{0.03f ,0.0f, 0.05},
		{0.03f ,0.0f, -0.05},

		{-0.03f ,0.0f, 0.05},
		{-0.03f ,0.0f, -0.05},
	};

	std::vector<glm::vec3> extraPoints;

	lines.SetColour({ 0.0f, 1.0f, 0.0f });
	for (int i = 0; i < robot.size(); i++)
	{
		glm::vec4 temp = (r->transform()->getGlobalMatrix() * glm::vec4(robot[i], 1.0f));
		extraPoints.push_back({ temp.x, temp.y, temp.z });
		if(i < 4)
			lines.AddPointToLine({ temp.x, temp.y, temp.z });
	}
	lines.FinishLineLoop();
	glm::vec3 otherPos1 = extraPoints[4] + glm::vec3(wheelDirection.x, 0.0f, wheelDirection.y) * 0.04f;
	glm::vec3 otherPos2 = extraPoints[5] + glm::vec3(wheelDirection.x, 0.0f, wheelDirection.y) * 0.04f;
	glm::vec3 otherPos3 = extraPoints[6] - r->transform()->forward() * 0.04f;
	glm::vec3 otherPos4 = extraPoints[7] - r->transform()->forward() * 0.04f;
	lines.DrawLineSegment(extraPoints[4], otherPos1);
	lines.DrawLineSegment(extraPoints[5], otherPos2);
	lines.DrawLineSegment(extraPoints[6], otherPos3);
	lines.DrawLineSegment(extraPoints[7], otherPos4);
	


	for (int y = 0; y < level.height - 1; y++)
	{
		for (int x = 0; x < level.width - 1; x++)
		{
			auto at = level.getValueCompAt(x, y);
			auto right = level.getValueCompAt(x + 1, y);
			auto down = level.getValueCompAt(x, y + 1);
			
			bool wallHere = MapCellIs(at, 0, 0, 0);
			bool wallDown = MapCellIs(down, 0, 0, 0);
			bool wallRight = MapCellIs(right, 0, 0, 0);

			bool onNothing = false;

			int xPos = x;
			int yPos = level.height - y;

			if (wallHere)
			{
				if (!wallRight)
				{
					lines.DrawLineSegment({ xPos + halfGridSpacing, 0, yPos + halfGridSpacing }, { xPos + halfGridSpacing, 0, yPos - halfGridSpacing }, { 1, 0, 0 });
				}
				if (!wallDown)
				{
					lines.DrawLineSegment({ xPos - halfGridSpacing, 0, yPos - halfGridSpacing }, { xPos + halfGridSpacing, 0, yPos - halfGridSpacing }, { 0, 1, 0 });
				}
			}
			else
			{
				if (wallRight)
				{
					lines.DrawLineSegment({ xPos + halfGridSpacing, 0, yPos + halfGridSpacing }, { xPos + halfGridSpacing, 0, yPos - halfGridSpacing }, { 0, 0, 1 });
				}
				if (wallDown)
				{
					lines.DrawLineSegment({ xPos - halfGridSpacing, 0, yPos - halfGridSpacing }, { xPos + halfGridSpacing, 0, yPos - halfGridSpacing }, { 1, 1, 0 });
				}
			}
		}
	}

}

void GameTest::Draw()
{
	renderSystem->Update(
		renderers,
		transforms,
		renderers,
		animators,
		camera
	);
}

void GameTest::GUI()
{
	input.GUI();

	if (ImGui::Begin("Car Numbers")) {
		ImGui::DragFloat("Car move peed", &carMoveSpeed);
		ImGui::DragFloat("Max car move speed", &maxCarMoveSpeed);
		ImGui::DragFloat("Turning circle scalar", &turningCircleScalar);
		ImGui::DragFloat("Max wheel angle", &maxWheelAngle);
		ImGui::DragFloat("Wheel Turn Speed", &wheelTurnSpeed);
		ImGui::DragFloat("Sideways Wheel Drag", &sidewaysFrictionCoef);
		ImGui::DragFloat("Stopping Wheel Drag", &stoppingFrictionCoef);


		ImGui::BeginDisabled();

		ImGui::DragFloat(("Rotation"), &rot);
		ImGui::DragFloat2(("WheelDirection"), &wheelDirection[0]);

		ImGui::EndDisabled();
	}
	ImGui::End();
}

GameTest::~GameTest()
{
}
