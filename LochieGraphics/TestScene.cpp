#include "TestScene.h"

#include "MultiModelRenderer.h"

#include "stb_image.h"

#include <iostream>

TestScene::TestScene()
{
	sceneObjects = std::vector<SceneObject*>{
		boxes,
		grass,
		lightCube,
		backpack,
		soulSpear,
		testRedBox,
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
	litNormalShader  = ResourceManager::GetShader("shaders/litNormal.vert", "shaders/litNormal.frag");
	litShader		 = ResourceManager::GetShader("shaders/lit.vert",		"shaders/lit.frag");
	lightCubeShader  = ResourceManager::GetShader("shaders/lightCube.vert", "shaders/lightCube.frag");
	skyBoxShader	 = ResourceManager::GetShader("shaders/cubemap.vert",	"shaders/cubemap.frag");
	skybox.InitialiseCubeInsideOut();

	std::vector<std::string> faces = std::vector<std::string>{
		//"images/otherskybox/right.png",
		//"images/otherskybox/left.png",
		//"images/otherskybox/top.png",
		//"images/otherskybox/bottom.png",
		//"images/otherskybox/front.png",
		//"images/otherskybox/back.png"
		"images/skybox/right.jpg",
		"images/skybox/left.jpg",
		"images/skybox/top.jpg",
		"images/skybox/bottom.jpg",
		"images/skybox/front.jpg",
		"images/skybox/back.jpg"
	};
	skyboxTexture = Texture::LoadCubeMap(faces); //TODO: Should be using the resource manager

	litShader->Use();
	litShader->setFloat("material.shininess", 32.f);

	litNormalShader->Use();
	litNormalShader->setFloat("material.shininess", 32.f);

	Mesh cubeMesh;
	cubeMesh.InitialiseCube();
	Material* boxMaterial = ResourceManager::GetMaterial("box", litShader);
	boxMaterial->AddTextures(std::vector<Texture*> {
		ResourceManager::GetTexture("images/container2.png", Texture::Type::diffuse),
		ResourceManager::GetTexture("images/container2_specular.png", Texture::Type::specular),
	});
	cubeModel.AddMesh(&cubeMesh);
	boxes->setRenderer(new MultiModelRenderer(&cubeModel, boxMaterial, std::vector<Transform>
	{
		Transform({  0.0f,  0.0f,  0.00f }, { 00.f,   0.f, 00.f }),
		Transform({  2.0f,  5.0f, -15.0f }, { 20.f,   6.f, 10.f }),
		Transform({ -1.5f, -2.2f, -2.50f }, { 40.f,  12.f, 20.f }),
		Transform({ -3.8f, -2.0f, -12.3f }, { 60.f,  18.f, 30.f }),
		Transform({  2.4f, -0.4f, -3.50f }, { 80.f,  24.f, 40.f }),
		Transform({ -1.7f,  3.0f, -7.50f }, { 100.f, 30.f, 50.f }),
		Transform({  1.3f, -2.0f, -2.50f }, { 120.f, 36.f, 60.f }),
		Transform({  1.5f,  2.0f, -2.50f }, { 140.f, 42.f, 70.f }),
		Transform({  1.5f,  0.2f, -1.50f }, { 160.f, 48.f, 80.f }),
		Transform({ -1.3f,  1.0f, -1.50f }, { 180.f, 54.f, 90.f })
	}));

	Material* lightCubeMaterial = ResourceManager::GetMaterial("lightCube", lightCubeShader);
	MultiModelRenderer* lightCubeRenderer = new MultiModelRenderer(&cubeModel, lightCubeMaterial, std::vector<Transform>(sizeof(pointLights) / sizeof(pointLights[0])));
	lightCube->setRenderer(lightCubeRenderer);
	for (int i = 0; i < sizeof(pointLights) / sizeof(pointLights[0]); i++)
	{
		lightCubeRenderer->transforms[i].scale = 0.2f;
	}

	Mesh grassMesh;
	grassMesh.InitialiseDoubleSidedQuad();
	Material* grassMaterial = ResourceManager::GetMaterial("grass", litShader);
	grassMaterial->AddTextures(std::vector<Texture*>{
		ResourceManager::GetTexture("images/grass.png", Texture::Type::diffuse, GL_CLAMP_TO_EDGE, false),
	});
	grassModel.AddMesh(&grassMesh);
	grass->setRenderer(new MultiModelRenderer(&grassModel, grassMaterial, std::vector<Transform>{
		Transform({ 0.0f,  0.0f,   0.0f }, { 0.f,  20.f, 0.f }),
		Transform({ 2.0f,  0.0f, -15.0f }, { 0.f,  40.f, 0.f }),
		Transform({ -1.5f, 0.0f, -03.5f }, { 0.f,  60.f, 0.f }),
		Transform({ -3.8f, 0.0f, -10.3f }, { 0.f,  80.f, 0.f }),
		Transform({ 2.4f,  0.0f, -05.5f }, { 0.f, 100.f, 0.f }),
		Transform({ -1.7f, 0.0f, -09.5f }, { 0.f, 120.f, 0.f }),
		Transform({ 1.3f,  0.0f, -03.5f }, { 0.f, 140.f, 0.f }),
		Transform({ 1.5f,  0.0f, -03.5f }, { 0.f, 160.f, 0.f }),
		Transform({ 1.5f,  0.0f, -04.5f }, { 0.f, 180.f, 0.f }),
		Transform({ -1.3f, 0.0f, -01.5f }, { 0.f, 200.f, 0.f }),
	}));

	backpackModel = Model("models/backpack/backpack.obj", false);
	Material* backpackMaterial = ResourceManager::GetMaterial("backpack", litNormalShader);
	backpackMaterial->AddTextures(std::vector<Texture*>{
		ResourceManager::GetTexture("models/backpack/diffuse.jpg", Texture::Type::diffuse, GL_REPEAT, false),
		ResourceManager::GetTexture("models/backpack/normal.png", Texture::Type::normal, GL_REPEAT, false),
		ResourceManager::GetTexture("models/backpack/specular.jpg", Texture::Type::specular, GL_REPEAT, false)
	});
	backpack->setRenderer(new ModelRenderer(&backpackModel, backpackMaterial));
	backpack->transform.position = { -5.f, -1.f, 0.f };

	testRedBoxModel = Model("models/normalBoxTest/Box_normal_example.obj");
	Material* testRedBoxMaterial = ResourceManager::GetMaterial("testRedBox", litNormalShader);
	testRedBoxMaterial->AddTextures(std::vector<Texture*>{
		ResourceManager::GetTexture("models/normalBoxTest/box_example_None_BaseColor.png", Texture::Type::diffuse, GL_REPEAT, true),
		ResourceManager::GetTexture("models/normalBoxTest/box_example_None_Normal.png", Texture::Type::normal, GL_REPEAT, true),
	});
	testRedBox->setRenderer(new ModelRenderer(&testRedBoxModel, testRedBoxMaterial));
	testRedBox->transform.position = { 5.f, -3.f, 2.f };

	soulSpearModel = Model(std::string("models/soulspear/soulspear.obj"), true);
	Material* soulSpearMaterial = ResourceManager::GetMaterial("soulSpear", litNormalShader);
	soulSpearMaterial->AddTextures(std::vector<Texture*>{
		ResourceManager::GetTexture("models/soulspear/soulspear_diffuse.tga", Texture::Type::diffuse, GL_REPEAT, true),
		ResourceManager::GetTexture("models/soulspear/soulspear_specular.tga", Texture::Type::specular, GL_REPEAT, true),
		ResourceManager::GetTexture("models/soulspear/soulspear_normal.tga", Texture::Type::normal, GL_REPEAT, true)
		//ResourceManager::GetTexture("images/DefaultNormal.png", Texture::Type::normal, GL_REPEAT, true)
	});
	soulSpear->setRenderer(new ModelRenderer(&soulSpearModel, soulSpearMaterial));
	soulSpear->transform.position = { 0.f, 1.f, 1.f };
}

void TestScene::Update(float delta)
{
	messengerInterface.Update();

	pointLights[0].position.x = 1.5f * sin(glfwGetTime() * 2.f);
	spotlight.position = camera->position;
	spotlight.direction = camera->front;
	for (auto i = lights.begin(); i != lights.end(); i++)
	{
		(*i)->ApplyToShader(litNormalShader);
		(*i)->ApplyToShader(litShader);
	}

	MultiModelRenderer* lightCubeRenderer = (MultiModelRenderer*)lightCube->getRenderer();
	for (int i = 0; i < sizeof(pointLights) / sizeof(pointLights[0]); i++)
	{
		lightCubeRenderer->transforms[i].position = pointLights[i].position;
	}
	
	// Shader
	// TODO: Is there some way of having a shader system where if shaders have certain uniforms they get set, this could get messy with more shaders
	litNormalShader->Use();
	litNormalShader->setVec3("viewPos", camera->position);
	litNormalShader->setMat4("vp", viewProjection);

	litShader->Use();
	litShader->setVec3("viewPos", camera->position);
	litShader->setMat4("vp", viewProjection);

	lightCubeShader->Use();
	lightCubeShader->setMat4("vp", viewProjection);

	skyBoxShader->Use();

	// Different View Projection matrix for the skybox, as translations shouldn't affect it
	glm::mat4 view = glm::mat4(glm::mat3(camera->GetViewMatrix()));
	glm::mat4 projection = glm::perspective(glm::radians(camera->fov), (float)*windowWidth / (float)*windowHeight, 0.01f, 100.0f);
	glm::mat4 vp = projection * view;
	skyBoxShader->setMat4("vp", vp);


	// TODO: Actual draw/update loop
	for (auto i = sceneObjects.begin(); i != sceneObjects.end(); i++)
	{
		(*i)->Update(delta);
		(*i)->Draw();
	}
	// Skybox, 
	// TODO: Make a skybox sceneobject or something, the skybox stuff needs its own space/class

	glDepthFunc(GL_LEQUAL); // Change depth function
	Texture::UseCubeMap(skyboxTexture, skyBoxShader);
	skybox.Draw(); // Actually draw the skyBox
	glDepthFunc(GL_LESS); // Change depth function back
}

void TestScene::GUI()
{
	messengerInterface.GUI();
}

TestScene::~TestScene()
{
}
