#include "TestScene.h"

#include "MultiModelRenderer.h"

#include "stb_image.h"

TestScene::TestScene()
{
	sceneObjects = std::vector<SceneObject*>{
		boxes,
		grass,
		lightCube,
		backpack
	};
	lights = std::vector<Light*>{
		&pointLights[0],
		&pointLights[1],
		&pointLights[2],
		&pointLights[3],
		&spotlight,
		&directionalLight
	};
	
}

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
	boxes->AddPart(new MultiModelRenderer(&boxModel, lightingShader, std::vector<Transform>
	{
		Transform({  0.0f,  0.0f,  0.00f }, { 20.f,   6.f,  10.f }, 1.0f),
		Transform({  2.0f,  5.0f, -15.0f }, { 40.f,  12.f,  20.f }, 1.0f),
		Transform({ -1.5f, -2.2f, -2.50f }, { 60.f,  18.f,  30.f }, 1.0f),
		Transform({ -3.8f, -2.0f, -12.3f }, { 80.f,  24.f,  40.f }, 1.0f),
		Transform({  2.4f, -0.4f, -3.50f }, { 100.f, 30.f,  50.f }, 1.0f),
		Transform({ -1.7f,  3.0f, -7.50f }, { 120.f, 36.f,  60.f }, 1.0f),
		Transform({  1.3f, -2.0f, -2.50f }, { 140.f, 42.f,  70.f }, 1.0f),
		Transform({  1.5f,  2.0f, -2.50f }, { 160.f, 48.f,  80.f }, 1.0f),
		Transform({  1.5f,  0.2f, -1.50f }, { 180.f, 54.f,  90.f }, 1.0f),
		Transform({ -1.3f,  1.0f, -1.50f }, { 200.f, 60.f, 100.f }, 1.0f)
	}));

	Mesh lightCubeMesh;
	lightCubeMesh.InitialiseCube();
	lightCubeModel.AddMesh(&lightCubeMesh);
	lightCube->AddPart(new ModelRenderer(&lightCubeModel, lightCubeShader));
	lightCube->transform.scale = 0.2f;

	Mesh grassMesh;
	grassMesh.InitialiseDoubleSidedQuad();
	grassMesh.material = ResourceManager::GetMaterial(std::vector<Texture*>{
		ResourceManager::GetTexture("images/grass.png", Texture::Type::diffuse, GL_CLAMP_TO_EDGE)
	});
	grassModel.AddMesh(&grassMesh);
	grass->AddPart(new MultiModelRenderer(&grassModel, lightingShader, std::vector<Transform>{
		Transform({ 0.0f, 0.0f,   0.0f }, { 0.f,  20.f, 0.f }, 1.0f),
		Transform({ 2.0f, 0.0f, -15.0f }, { 0.f,  40.f, 0.f }, 1.0f),
		Transform({ -1.5f, 0.0f, -03.5f }, { 0.f,  60.f, 0.f }, 1.0f),
		Transform({ -3.8f, 0.0f, -10.3f }, { 0.f,  80.f, 0.f }, 1.0f),
		Transform({ 2.4f, 0.0f, -05.5f }, { 0.f, 100.f, 0.f }, 1.0f),
		Transform({ -1.7f, 0.0f, -09.5f }, { 0.f, 120.f, 0.f }, 1.0f),
		Transform({ 1.3f, 0.0f, -03.5f }, { 0.f, 140.f, 0.f }, 1.0f),
		Transform({ 1.5f, 0.0f, -03.5f }, { 0.f, 160.f, 0.f }, 1.0f),
		Transform({ 1.5f, 0.0f, -04.5f }, { 0.f, 180.f, 0.f }, 1.0f),
		Transform({ -1.3f, 0.0f, -01.5f }, { 0.f, 200.f, 0.f }, 1.0f),
	}));

	//backpackModel = Model("models/backpack/backpack.obj", false);
	backpackModel = Model(std::string("models/soulspear/soulspear.obj"), false);
	backpackModel.SetMaterial(ResourceManager::GetMaterial(std::vector<Texture*>{
		ResourceManager::GetTexture("models/soulspear/soulspear_diffuse.tga", Texture::Type::diffuse),
		ResourceManager::GetTexture("models/soulspear/soulspear_specular.tga", Texture::Type::specular),
		ResourceManager::GetTexture("models/soulspear/soulspear_normal.tga", Texture::Type::normal)
	}));
	backpack->AddPart(new ModelRenderer(&backpackModel, lightingShader));
	backpack->transform.position = { 0.f, 1.f, 1.f };
}

void TestScene::Update(float delta)
{
	spotlight.position = camera->position;
	spotlight.direction = camera->front;
	for (auto i = lights.begin(); i != lights.end(); i++)
	{
		(*i)->ApplyToShader(lightingShader);
	}
	
	// Shader
	lightingShader->Use();
	lightingShader->setVec3("viewPos", camera->position);
	lightingShader->setMat4("vp", viewProjection);

	lightCubeShader->Use();
	lightCubeShader->setMat4("vp", viewProjection);

	// Drawing
	// TODO: Actual draw/update loop
	for (auto i = sceneObjects.begin(); i != sceneObjects.end(); i++)
	{
		(*i)->Update(delta);
		(*i)->Draw();
	}
}

TestScene::~TestScene()
{
}
