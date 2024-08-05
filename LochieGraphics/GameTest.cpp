#include "GameTest.h"
#include "Utilities.h"
#include "ResourceManager.h"

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
	rRb->setMass(1.0f);
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
}

void GameTest::Update(float delta)
{
	input.Update();

	LineRenderer& lines = renderSystem->lines;

	physicsSystem.UpdateRigidBodies(transforms, rigidbodies, delta);

	

	if (input.inputters.size() > 0) {
		glm::vec2 move = { (input.inputters[0]->getRightTrigger() + 1.0f) / 2.0f, input.inputters[0]->getMove().x};
		glm::vec3 force = move.x * 30.0f * h->transform()->forward();
		hRb->netForce += glm::vec2(force.x, force.z);
		float rotation = move.y;
		hRb->angularVel = rotation;
	}
	







	// Draw human
	glm::vec3 hPos = h->transform()->getPosition();
	rot = h->transform()->getEulerRotation().y / 180 * PI;
	rotary = h->transform()->getEulerRotation();

	lines.SetColour({ 0.0f, 1.0f, 0.0f });
	lines.AddPointToLine({ hPos.x - hRadius, hPos.y, hPos.z - hRadius });
	lines.AddPointToLine({ hPos.x - hRadius, hPos.y, hPos.z + hRadius });
	lines.AddPointToLine({ hPos.x + hRadius, hPos.y, hPos.z + hRadius });
	lines.AddPointToLine({ hPos.x + hRadius, hPos.y, hPos.z - hRadius });
	lines.FinishLineLoop();
	glm::vec3 otherPos = hPos + (hRadius * glm::vec3{ glm::sign(cosf(rotary.x)) * cosf(rot), 0.0f, -sinf(rot) });

	lines.DrawLineSegment(hPos, otherPos);



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

		ImGui::DragFloat3(("Rotation as Vec"), &rotary[0]);

		ImGui::EndDisabled();
	}
	ImGui::End();
}

GameTest::~GameTest()
{
}
