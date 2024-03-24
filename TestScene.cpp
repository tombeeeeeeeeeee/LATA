#include "TestScene.h"

#include "imguiStuff.h"

void TestScene::Start()
{
	// Shaders
	lightingShader = ResourceManager::GetShader("shader.vert", "shader.frag");
	lightCubeShader = ResourceManager::GetShader("lightCube.vert", "lightCube.frag");

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

	// Drawing
	backpack.Draw();

	for (unsigned int i = 0; i < 10; i++)
	{
		box.position = cubePositions[i];
		box.rotation = { 20.f * i, 6.f * i, 10.f * i };
		box.Draw();
	}

	for (unsigned int i = 0; i < 10; i++)
	{
		grass.rotation.y = 20.f * i;
;		grass.position = grassPositions[i];
		grass.Draw();
	}

	for (unsigned int i = 0; i < 4; i++)
	{
		lightCube.position = pointLights[i].position;
		lightCube.Draw();
	}	
}

TestScene::~TestScene()
{
	
}
