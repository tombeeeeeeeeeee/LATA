#include "TestScene.h"

#include "imguiStuff.h"

TestScene::TestScene()
{
	sceneObjects = std::vector<SceneObject*>{
		&box,
		&grass,
		&lightCube,
		&backpack
	};
}

void TestScene::Start()
{
	// Shaders
	lightingShader = ResourceManager::GetShader("shader.vert", "shader.frag");
	lightCubeShader = ResourceManager::GetShader("lightCube.vert", "lightCube.frag");
	solidColourShader = ResourceManager::GetShader("lightCube.vert", "solidColour.frag");

	solidColourShader->Use();
	solidColourShader->setVec3("colour", {0.2f, 1.0f, 0.2f});

	lightingShader->Use();
	lightingShader->setFloat("material.shininess", 64.0f);

	directionalLight.ApplyToShader(lightingShader);
	spotlight.ApplyToShader(lightingShader);
	for (int i = 0; i < sizeof(pointLights) / sizeof(pointLights[0]); i++)
	{
		pointLights[i].ApplyToShader(lightingShader);
	}

	Mesh boxMesh;
	boxMesh.InitialiseCube();
	boxMesh.material = ResourceManager::GetMaterial(std::vector<Texture*> { 
		ResourceManager::GetTexture("images/container2.png", Texture::Type::diffuse),
		ResourceManager::GetTexture("images/container2_specular.png", Texture::Type::specular)
	});
	boxModel.AddMesh(&boxMesh);
	box = SceneObject(&boxModel, lightingShader);

	Mesh lightCubeMesh;
	lightCubeMesh.InitialiseCube();
	lightCubeModel.AddMesh(&lightCubeMesh);
	lightCube = SceneObject(&lightCubeModel, lightCubeShader, { 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f }, 0.2f);

	Mesh grassMesh;
	grassMesh.InitialiseDoubleSidedQuad();
	grassMesh.material = ResourceManager::GetMaterial(std::vector<Texture*>{ 
		ResourceManager::GetTexture("images/grass.png", Texture::Type::diffuse, GL_CLAMP_TO_EDGE)
	});
	grassModel.AddMesh(&grassMesh);
	grass = SceneObject(&grassModel, lightingShader);

	backpackModel = Model("models/backpack/backpack.obj");
	backpack = SceneObject(&backpackModel, lightingShader, { 0.f, 1.f, 1.f });

	glEnable(GL_STENCIL_TEST);

}

void TestScene::Update(float delta)
{
	// Shader
	lightingShader->Use();
	lightingShader->setVec3("spotlight.position", camera->position);
	lightingShader->setVec3("spotlight.direction", camera->front);
	lightingShader->setVec3("viewPos", camera->position);
	lightingShader->setMat4("vp", viewProjection);

	lightCubeShader->Use();
	lightCubeShader->setMat4("vp", viewProjection);
	solidColourShader->Use();
	solidColourShader->setMat4("vp", viewProjection);

	// Stencil stuff
	glEnable(GL_DEPTH_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	// Disable Drawing to the stencil mask
	glStencilMask(0x00);


	// Drawing
	backpack.Draw();

	// Enable Drawing to the stencil mask
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);

	box.shader = lightingShader;
	box.transform.scale = 1.0f;
	for (unsigned int i = 0; i < 10; i++)
	{
		box.transform.position = cubePositions[i];
		box.transform.rotation = { 20.f * i, 6.f * i, 10.f * i };
		box.Draw();
	}

	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
	glDisable(GL_DEPTH_TEST);



	box.shader = solidColourShader;
	box.transform.scale = 1.1f;
	for (unsigned int i = 0; i < 10; i++)
	{
		box.transform.position = cubePositions[i];
		box.transform.rotation = { 20.f * i, 6.f * i, 10.f * i };
		box.Draw();
	}

	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glEnable(GL_DEPTH_TEST);


	for (unsigned int i = 0; i < 10; i++)
	{
		grass.transform.rotation.y = 20.f * i;
;		grass.transform.position = grassPositions[i];
		grass.Draw();
	}

	for (unsigned int i = 0; i < 4; i++)
	{
		lightCube.transform.position = pointLights[i].position;
		lightCube.Draw();
	}	
}

TestScene::~TestScene()
{
	
}
