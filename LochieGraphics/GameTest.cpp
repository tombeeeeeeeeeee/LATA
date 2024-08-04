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
	hRb->setInvMomentOfInertia(0.25f);

	rigidbodies[r->GUID] = RigidBody();
	rRb = &rigidbodies[r->GUID];
	rRb->setMass(1.0f);
	rRb->setInvMomentOfInertia(0.25f);

	input.Initialise();
}

void GameTest::Update(float delta)
{
	input.Update();

	LineRenderer& lines = renderSystem->lines;

	physicsSystem.UpdateRigidBodies(transforms, rigidbodies, delta);

	

	if (input.inputters.size() >= 1) {
		glm::vec2 move = (input.inputters[1]->getMove());
		hRb->addForce(move);
		hRb->AddRotationalImpulse(100000.1f);
	}
	







	// Draw human
	glm::vec3 hPos = h->transform()->getPosition();
	
	lines.SetColour({ 0.0f, 1.0f, 0.0f });
	lines.AddPointToLine({ hPos.x - hRadius, hPos.y, hPos.z - hRadius });
	lines.AddPointToLine({ hPos.x - hRadius, hPos.y, hPos.z + hRadius });
	lines.AddPointToLine({ hPos.x + hRadius, hPos.y, hPos.z + hRadius });
	lines.AddPointToLine({ hPos.x + hRadius, hPos.y, hPos.z - hRadius });
	lines.FinishLineLoop();



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
