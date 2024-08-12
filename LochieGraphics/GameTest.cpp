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
	hRb->addCollider({ new PolygonCollider({{0.0f, 0.0f}}, hRadius)});
	hRb->setMomentOfInertia(5.0f);

	rRb = new RigidBody();
	rRb->addCollider({new PolygonCollider(
			{
				{hRadius, hRadius}, 
				{hRadius, -hRadius},
				{-hRadius, -hRadius},
				{-hRadius, hRadius},
			}, 0.0f) }
	);
	rRb->setMass(0.1f);
	rRb->setMomentOfInertia(5.0f);

	h->transform()->setPosition({ 1.0f,0.0f,1.0f });

	input.Initialise();

	h->setRigidBody(hRb);
	r->setRigidBody(rRb);

	hRb = &rigidBodies[h->GUID];
	rRb = &rigidBodies[r->GUID];
	r->setEcco(new Ecco());
	ecco->wheelDirection = {r->transform()->forward().x, r->transform()->forward().y};

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
		ecco->Update(
			*input.inputDevices[0],
			*r->transform(),
			*r->rigidbody(),
			delta
		);
	}

	physicsSystem.CollisionCheckPhase(transforms, rigidBodies, colliders);

	// Draw robot
	glm::vec3 rPos = r->transform()->getPosition();
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
	glm::vec3 otherPos1 = extraPoints[4] + glm::vec3(ecco->wheelDirection.x, 0.0f, ecco->wheelDirection.y) * 0.04f;
	glm::vec3 otherPos2 = extraPoints[5] + glm::vec3(ecco->wheelDirection.x, 0.0f, ecco->wheelDirection.y) * 0.04f;
	glm::vec3 otherPos3 = extraPoints[6] - r->transform()->forward() * 0.04f;
	glm::vec3 otherPos4 = extraPoints[7] - r->transform()->forward() * 0.04f;
	lines.DrawLineSegment(extraPoints[4], otherPos1);
	lines.DrawLineSegment(extraPoints[5], otherPos2);
	lines.DrawLineSegment(extraPoints[6], otherPos3);
	lines.DrawLineSegment(extraPoints[7], otherPos4);
	
	lines.DrawCircle(h->transform()->getGlobalPosition(), hRadius, 40);
	


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

	ecco->GUI();

	//ImGui::End();
}

GameTest::~GameTest()
{
}
