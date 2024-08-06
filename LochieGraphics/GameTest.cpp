#include "GameTest.h"
#include "Utilities.h"
#include "ResourceManager.h"

#include "Image.h"

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
	
	rigidbodies[h->GUID] = RigidBody();
	hRb = &rigidbodies[h->GUID];
	hRb->setMass(1.0f);
	hRb->setMomentOfInertia(5.0f);

	rigidbodies[r->GUID] = RigidBody();
	rRb = &rigidbodies[r->GUID];
	rRb->setMass(0.1f);
	rRb->setMomentOfInertia(5.0f);

	input.Initialise();

	sceneObjects.insert(sceneObjects.end(), { h, r });
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


}

void GameTest::Update(float delta)
{
	input.Update();

	LineRenderer& lines = renderSystem->lines;

	physicsSystem.UpdateRigidBodies(transforms, rigidbodies, delta);

	if (input.inputters.size() > 0) {

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
		
		Input::Inputter* rC = input.inputters[0];

		glm::vec2 tireTurnDirection = rC->getMove();
		//float turnAmount = glm::dot(glm::vec2(tireTurnDirection.y, -tireTurnDirection.x), wheelDirection);
		//tireTurnDirection = tireTurnDirection /*+ turnAngleMod*/ * turnAmount /** tile*/;
		//

		//glm::vec2 move(Utilities::mapValueTo(rC->getRightTrigger(), -1.0f, 1.0f, 0.0f, 1.0f) - Utilities::mapValueTo(rC->getLeftTrigger(), -1.0f, 1.0f, 0.0f, 1.0f), 
		//	rC->getMove().x);
		//glm::vec3 force = move.x * carMoveSpeed * r->transform()->forward();
		////rRb->netForce += glm::vec2(force.x, force.z);
		//rRb->netForce += force * tireTurnDirection;
		//float rotation = move.y;
		//rRb->angularVel = rotation;

		float forward = Utilities::mapValueTo(rC->getRightTrigger(), -1.0f, 1.0f, 0.0f, 1.0f) - Utilities::mapValueTo(rC->getLeftTrigger(), -1.0f, 1.0f, 0.0f, 1.0f);
		
		// Make the RB have a forward that is a vec 2
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
		std::cout << '\n';
		for (int x = 0; x < level.width - 1; x++)
		{
			auto at = level.getValueCompAt(x, y);
			auto right = level.getValueCompAt(x + 1, y);
			auto down = level.getValueCompAt(x, y + 1);
			//auto left = level.getValueCompAt(x - 1, y);
			auto up = level.getValueCompAt(x, y - 1);

			bool wallHere = (at[0] == 0) && (at[1] == 0) && (at[2] == 0);
			bool wallDown = (down[0] == 0) && (down[1] == 0) && (down[2] == 0);
			bool wallRight = (right[0] == 0) && (right[1] == 0) && (right[2] == 0);
			//bool wallUp = *up == 0;
			//bool wallLeft = *left== 0;



			bool onNothing = false;
			// Black
			//} && (at[1] == 0) && (at[2] == 0)) {
			if (*at == 0)
			{
				onNothing = true;
				std::cout << '*';
			}
			else
			{
				std::cout << ' ';
			}

			int xPos = x;
			int yPos = level.height - y;

			if (wallHere)
			{
				if (!wallRight)
				{
					renderSystem->lines.DrawLineSegment({ xPos + 0.5, 0, yPos + 0.5 }, { xPos + 0.5, 0, yPos - 0.5 }, { 1, 0, 0 });
				}
				if (!wallDown)
				{
					renderSystem->lines.DrawLineSegment({ xPos - 0.5, 0, yPos - 0.5 }, { xPos + 0.5, 0, yPos - 0.5 }, { 0, 1, 0 });
				}
			}
			else
			{
				if (wallRight)
				{
					renderSystem->lines.DrawLineSegment({ xPos + 0.5, 0, yPos + 0.5 }, { xPos + 0.5, 0, yPos - 0.5 }, { 0, 0, 1 });
				}
				if (wallDown)
				{
					renderSystem->lines.DrawLineSegment({ xPos - 0.5, 0, yPos - 0.5 }, { xPos + 0.5, 0, yPos - 0.5 }, { 1, 1, 0 });
				}
			}

			renderSystem->lines.DrawLineSegment({ 0, 0, 0 }, { 10, 0, 10 }, {0.5, 0.5, 0.5});

			//if (((right[0] == 0) && (right[1] == 0) && (right[2] == 0)) != onNothing) {
			//	renderSystem->lines.DrawLineSegment({ x + 0.5, 0, y + 0.5 }, { x + 0.5, 0, y - 0.5 }, { 1, 1, 1 });
			//}
			//if (((down[0] == 0) && (down[1] == 0) && (down[2] == 0)) != onNothing) {
			//	renderSystem->lines.DrawLineSegment({ x - 0.5, 0, y - 0.5 }, { x + 0.5, 0, y - 0.5 }, { 1, 1, 1 });
			//}
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
	if (ImGui::Begin("Input Debug")) {
		if (ImGui::Button("Show all controller connections")) {
			input.ShowAllControllerSlotStatuses();
		}
		ImGui::BeginDisabled();
		for (Input::Inputter* i : input.inputters)
		{
			glm::vec2 move = i->getMove();
			glm::vec2 look = i->getLook();
			ImGui::DragFloat2(("Move##" + Utilities::PointerToString(i)).c_str(), &(move.x));
			//ImGui::SameLine();
			ImGui::DragFloat2(("Look##" + Utilities::PointerToString(i)).c_str(), &(look.x));
		}

		ImGui::DragFloat(("Rotation"), &rot);

		ImGui::DragFloat2(("WheelDirection"), &wheelDirection[0]);

		ImGui::EndDisabled();
	}
	ImGui::End();
}

GameTest::~GameTest()
{
}
