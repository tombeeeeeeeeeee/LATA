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

		glm::vec2 tireTurnDirection = rC->getMove();
		//float turnAmount = glm::dot(glm::vec2(tireTurnDirection.y, -tireTurnDirection.x), wheelDirection);
		//tireTurnDirection = tireTurnDirection /*+ turnAngleMod*/ * turnAmount /** tile*/;
		//

		//glm::vec2 move(rC->getRightTrigger() - rC->getLeftTrigger(), 
		//	rC->getMove().x);
		//glm::vec3 force = move.x * carMoveSpeed * r->transform()->forward();
		////rRb->netForce += glm::vec2(force.x, force.z);
		//rRb->netForce += force * tireTurnDirection;
		//float rotation = move.y;
		//rRb->angularVel = rotation;

		float forward = rC->getRightTrigger() - rC->getLeftTrigger();
		
		// TODO: Make the RB have a forward that is a float
		glm::vec3 temp = r->transform()->forward();
		rRb->vel += glm::vec2(temp.x, temp.z) * forward;


	}
	
	ImGui::DragFloat("Car move speed", &carMoveSpeed);



	// Draw robot
	glm::vec3 rPos = r->transform()->getPosition();
	rot = r->transform()->getEulerRotation().y / 180 * PI;
	glm::vec3 rotary = r->transform()->getEulerRotation();
	lines.SetColour({ 0.0f, 1.0f, 0.0f });
	lines.AddPointToLine({ rPos.x - hRadius, rPos.y, rPos.z - hRadius });
	lines.AddPointToLine({ rPos.x - hRadius, rPos.y, rPos.z + hRadius });
	lines.AddPointToLine({ rPos.x + hRadius, rPos.y, rPos.z + hRadius });
	lines.AddPointToLine({ rPos.x + hRadius, rPos.y, rPos.z - hRadius });
	lines.FinishLineLoop();
	glm::vec3 otherPos = rPos + (hRadius * glm::vec3{ glm::sign(cosf(rotary.x)) * cosf(rot), 0.0f, -sinf(rot) });

	lines.DrawLineSegment(rPos, otherPos);



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

			lines.DrawLineSegment({ 0, 0, 0 }, { 10, 0, 10 }, {0.5, 0.5, 0.5});
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

	if (ImGui::Begin("Game Test Debug")) {

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
