#include "GameTest.h"

#include "Skybox.h"
#include "ResourceManager.h"
#include "Image.h"
#include "SceneObject.h"
#include "Collider.h"
#include "Camera.h"
#include "Ecco.h"
#include "Sync.h"

#include "Utilities.h"

#include "EditorGUI.h"

#include <array>

bool GameTest::MapCellIs(unsigned char* cell, unsigned char r, unsigned char g, unsigned char b)
{
	return cell[0] == r && cell[1] == g && cell[2] == b;
}

GameTest::GameTest() :
	h(new SceneObject(this)),
	r(new SceneObject(this))
{
}

void GameTest::Start()
{
	directionalLight = DirectionalLight
	({ 1.0f, 1.0f, 1.0f }, { -0.533f, -0.533f, -0.533f });
	
	r = sceneObjects[r->GUID];
	h = sceneObjects[h->GUID];

	hRb = new RigidBody();
	hRb->setMass(1.0f);
	hRb->addCollider({ new PolygonCollider({{0.0f, 0.0f}}, hRadius, CollisionLayers::sync)});
	hRb->setMomentOfInertia(5.0f);

	rRb = new RigidBody();
	rRb->addCollider({new PolygonCollider(
			{
				{40.0f, 40.0f}, 
				{40.0f, -40.0f},
				{-40.0f, -40.0f},
				{-40.0f, 40.0f},
			}, 0.0f, CollisionLayers::ecco) }
	);
	rRb->setMass(0.1f);
	rRb->setMomentOfInertia(5.0f);

	h->transform()->setPosition({ 1.0f,0.0f,1.0f });

	input.Initialise();

	h->setRigidBody(hRb);
	r->setRigidBody(rRb);

	hRb = &rigidBodies[h->GUID];
	rRb = &rigidBodies[r->GUID];
	h->setSync(sync);
	h->setHealth(new Health());
	r->setEcco(ecco);
	r->setHealth(new Health());
	ecco->wheelDirection = {r->transform()->left().x, r->transform()->left().y};

	level.path = "level.png";
	level.Load();

	camera->state = Camera::targetingPlayersOrthographic;
	gameCamSystem.cameraPositionDelta = { -15,10,15 };

	Material* robotMaterial = ResourceManager::LoadMaterial("robot", ResourceManager::super);
	robotMaterial->AddTextures(std::vector<Texture*>{
		ResourceManager::LoadTexture("images/otherskybox/top.png", Texture::Type::albedo, GL_REPEAT, true),
			ResourceManager::LoadTexture("models/soulspear/soulspear_specular.tga", Texture::Type::PBR, GL_REPEAT, true),
			ResourceManager::LoadTexture("models/soulspear/soulspear_normal.tga", Texture::Type::normal, GL_REPEAT, true),
	});

	r->setRenderer(new ModelRenderer(
		ResourceManager::LoadModel("models/P2_Blockout.fbx"),
		robotMaterial
	));

	r->transform()->setScale(0.004f);
	camera->transform.setRotation(glm::quat(0.899f, -0.086f, 0.377f, -0.205f ));

	r->name = "Ecco";
	h->name = "Sync";

	sync->Start();

	SceneObject* newSceneObject = new SceneObject(this, "Walls");
	RigidBody* newRigidBody = new RigidBody(0.0f, 0.0f, {}, true);
	newSceneObject->setRigidBody(newRigidBody);

	for (int y = 0; y < level.height; y++)
	{
		for (int x = 0; x < level.width; x++)
		{
			auto at = level.getValueCompAt(x, y);
			auto right = level.getValueCompAt(x + 1, y);
			auto down = level.getValueCompAt(x, y + 1);

			bool wallHere = MapCellIs(at, 0, 0, 0);

			int xPos = x;
			int yPos = level.height - y;

			if (wallHere)
			{
				newSceneObject->rigidbody()->addCollider(new PolygonCollider({ 
					{x + 0.525f, yPos + 0.525f},  {x + 0.525f, yPos + -0.525f}, {x + -0.525f, yPos + -0.525f},  {x + -0.525f, yPos + 0.525f}, }, 0.0f));
			}
			else
			{
				h->transform()->setPosition(glm::vec3(xPos + 0.2f, 0.0f, yPos + 0.2f));
				r->transform()->setPosition(glm::vec3(xPos + 0.4f, 0.0f, yPos + 0.4f));
			}
		}
	}
	enemySystem.Start(transforms, rigidBodies, colliders);

	physicsSystem.SetCollisionLayerMask((int)CollisionLayers::sync, (int)CollisionLayers::sync, false);
}

void GameTest::Update(float delta)
{
	physicsSystem.CollisionCheckPhase(transforms, rigidBodies, colliders);

	physicsSystem.UpdateRigidBodies(transforms, rigidBodies, delta);

	healthSystem.Update(
		healths,
		renderers,
		delta
	);

	input.Update();
	if(ecco->GUID != 0)
	r = sceneObjects[ecco->GUID];
	if (sync->GUID != 0)
	h = sceneObjects[sync->GUID];

	if (firstFrame)
	{
		firstFrame = false;
		Skybox irradiance = Skybox(ResourceManager::skyBoxShader, Texture::LoadCubeMap(irradianceFaces.data()));
		renderSystem.SetIrradianceMap(irradiance.texture);
	}

	LineRenderer& lines = renderSystem.lines;

	if (singlePlayerMode)
	{
		if (input.inputDevices.size() > 0)
		{
			if (targettingP1)
			{
				sync->Update(
					*input.inputDevices[0],
					*h->transform(),
					*h->rigidbody(),
					&renderSystem.lines,
					delta,
					camera->transform.getEulerRotation().y
				);
			}
			else
			{
				ecco->Update(
					*input.inputDevices[0],
					*r->transform(),
					*r->rigidbody(),
					*r->health(),
					delta,
					camera->transform.getEulerRotation().y
				);
			}
		}
	}
	else
	{
		if (input.inputDevices.size() > 0)
		{
			ecco->Update(
				*input.inputDevices[0],
				*r->transform(),
				*r->rigidbody(),
				*r->health(),
				delta,
				camera->transform.getEulerRotation().y
			);

			if (input.inputDevices.size() > 1)
			{
				sync->Update(
					*input.inputDevices[1],
					*h->transform(),
					*h->rigidbody(),
					&renderSystem.lines,
					delta,
					camera->transform.getEulerRotation().y
				);
			}
		}
	}

	if (singlePlayerMode)
	{
		if (targettingP1)
			gameCamSystem.target = h->transform()->getGlobalPosition();
		else
			gameCamSystem.target = r->transform()->getGlobalPosition();
	}

	gameCamSystem.Update(
		*camera, *r->transform(), *h->transform(), singlePlayerZoom
	);


	// Draw robot
	glm::vec3 rPos = r->transform()->getPosition();
	glm::vec3 rotary = r->transform()->getEulerRotation();
	std::vector<glm::vec3> robot =
	{
		{ 0.0f - 40.0f, rPos.y, 0.0f - 40.0f },
		{ 0.0f - 40.0f, rPos.y, 0.0f + 40.0f },
		{ 0.0f + 40.0f, rPos.y, 0.0f + 40.0f },
		{ 0.0f + 40.0f, rPos.y, 0.0f - 40.0f },
		{20.f ,0.0f, 20.0f},
		{20.f ,0.0f, -20.0f},

		{-20.f ,0.0f, 20.0f},
		{-20.f ,0.0f, -20.0f},
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
	glm::vec3 otherPos3 = extraPoints[6] - r->transform()->left() * 0.04f;
	glm::vec3 otherPos4 = extraPoints[7] - r->transform()->left() * 0.04f;
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

void GameTest::Draw(float delta)
{
	renderSystem.Update(
		renderers,
		transforms,
		renderers,
		animators,
		pointLights,
		spotlights,
		camera,
		delta
	);
}

void GameTest::GUI()
{
	input.GUI();

	ecco->GUI();
	sync->GUI();

	gameCamSystem.GUI();

}

GameTest::~GameTest()
{
}
