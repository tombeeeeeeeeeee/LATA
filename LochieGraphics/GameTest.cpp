#include "GameTest.h"

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
	camera->UpdateVectors();
}

void GameTest::Update(float delta)
{
	input.Update();

	LineRenderer& lines = renderSystem->lines;

	physicsSystem.UpdateRigidBodies(transforms, rigidbodies, delta);

	

	if (input.inputters.size() >= 1) {
		glm::vec2 move = (input.inputters[1]->getMove());
		hRb->netForce += move * 30.0f;
		hRb->AddRotationalImpulse(0.01f);
	}
	







	// Draw human
	glm::vec3 hPos = h->transform()->getPosition();
	auto rot = h->transform()->getEulerRotation().y / 180 * PI;
	
	lines.SetColour({ 0.0f, 1.0f, 0.0f });
	lines.AddPointToLine({ hPos.x - hRadius, hPos.y, hPos.z - hRadius });
	lines.AddPointToLine({ hPos.x - hRadius, hPos.y, hPos.z + hRadius });
	lines.AddPointToLine({ hPos.x + hRadius, hPos.y, hPos.z + hRadius });
	lines.AddPointToLine({ hPos.x + hRadius, hPos.y, hPos.z - hRadius });
	lines.FinishLineLoop();
	glm::vec3 otherPos = hPos + (hRadius * glm::vec3{ cosf(rot), 0.0f, sinf(rot) });

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
}

GameTest::~GameTest()
{
}
