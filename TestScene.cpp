#include "TestScene.h"

#include "MultiModelRenderer.h"

TestScene::TestScene()
{
	sceneObjects = std::vector<SceneObject*>{
		&boxes,
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
	solidColourShader->setVec3("colour", { 0.2f, 1.0f, 0.2f });

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
	boxes.modelRenderer = new MultiModelRenderer(&boxModel, lightingShader, std::vector<Transform>
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
	});

	Mesh lightCubeMesh;
	lightCubeMesh.InitialiseCube();
	lightCubeModel.AddMesh(&lightCubeMesh);
	lightCube.modelRenderer = new ModelRenderer(&lightCubeModel, lightCubeShader);
	lightCube.transform.scale = 0.2f;

	Mesh grassMesh;
	grassMesh.InitialiseDoubleSidedQuad();
	grassMesh.material = ResourceManager::GetMaterial(std::vector<Texture*>{
		ResourceManager::GetTexture("images/grass.png", Texture::Type::diffuse, GL_CLAMP_TO_EDGE)
	});
	grassModel.AddMesh(&grassMesh);
	grass.modelRenderer = new MultiModelRenderer(&grassModel, lightingShader, std::vector<Transform>{
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
	});

	//backpackModel = Model("models/backpack/backpack.obj");
	backpackModel = Model("models/soulspear/soulspear.obj");
	backpackModel.SetMaterial(ResourceManager::GetMaterial(std::vector<Texture*>{
		ResourceManager::GetTexture("models/soulspear/soulspear_diffuse.tga", Texture::Type::diffuse),
		ResourceManager::GetTexture("models/soulspear/soulspear_specular.tga", Texture::Type::specular),
		ResourceManager::GetTexture("models/soulspear/soulspear_normal.tga", Texture::Type::normal)
	}));
	backpack.modelRenderer = new ModelRenderer(&backpackModel, lightingShader);
	backpack.transform.position = { 0.f, 1.f, 1.f };

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

	boxes.modelRenderer->shader = lightingShader;
	boxes.transform.scale = 1.0f;
	boxes.Draw();

	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
	glDisable(GL_DEPTH_TEST);

	boxes.modelRenderer->shader = solidColourShader;
	boxes.transform.scale = 1.1f;
	boxes.Draw();

	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glEnable(GL_DEPTH_TEST);

	grass.Draw();

	for (unsigned int i = 0; i < 4; i++)
	{
		lightCube.transform.position = pointLights[i].position;
		lightCube.Draw();
	}
}

TestScene::~TestScene()
{

}
