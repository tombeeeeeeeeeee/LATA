#include "TestScene.h"

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
		puppet,
		//vampire,
		//xbot
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
	litNormalShader = ResourceManager::LoadShader("shaders/litNormal.vert", "shaders/litNormal.frag", Shader::Flags::Lit | Shader::Flags::VPmatrix);
	litShader = ResourceManager::LoadShader("shaders/lit.vert", "shaders/lit.frag", Shader::Flags::Lit | Shader::Flags::VPmatrix);
	lightCubeShader = ResourceManager::LoadShader("shaders/lightCube.vert", "shaders/lightCube.frag", Shader::Flags::VPmatrix);
	skyBoxShader = ResourceManager::LoadShader("shaders/cubemap.vert", "shaders/cubemap.frag");
	animateShader = ResourceManager::LoadShader("shaders/animate.vert", "shaders/animate.frag", Shader::Flags::Animated);
	shaders = std::vector<Shader*>{ litNormalShader, litShader, lightCubeShader, skyBoxShader, animateShader };

	std::string skyboxFaces[6] = {
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
	//TODO: Should be using the resource manager
	skybox = new Skybox(skyBoxShader, Texture::LoadCubeMap(skyboxFaces));


	Material* boxMaterial = ResourceManager::LoadMaterial("box", litShader);
	boxMaterial->AddTextures(std::vector<Texture*> {
		ResourceManager::LoadTexture("images/container2.png", Texture::Type::diffuse),
			ResourceManager::LoadTexture("images/container2_specular.png", Texture::Type::specular),
	});
	cubeModel.AddMesh(new Mesh(Mesh::presets::cube));
	boxes->setRenderer(new ModelRenderer(&cubeModel, boxMaterial));

	Material* lightCubeMaterial = ResourceManager::LoadMaterial("lightCube", lightCubeShader);
	lightCube->setRenderer(new ModelRenderer(&cubeModel, lightCubeMaterial));
	lightCube->transform.scale = 0.2f;

	Material* grassMaterial = ResourceManager::LoadMaterial("grass", litShader);
	grassMaterial->AddTextures(std::vector<Texture*>{
		ResourceManager::LoadTexture("images/grass.png", Texture::Type::diffuse, GL_CLAMP_TO_EDGE, false),
	});
	grassModel.AddMesh(new Mesh(Mesh::presets::doubleQuad));
	grass->setRenderer(new ModelRenderer(&grassModel, grassMaterial));

	backpackModel = Model("models/backpack/backpack.obj", false);
	Material* backpackMaterial = ResourceManager::LoadMaterial("backpack", litNormalShader);
	backpackMaterial->AddTextures(std::vector<Texture*>{
		ResourceManager::LoadTexture("models/backpack/diffuse.jpg", Texture::Type::diffuse, GL_REPEAT, false),
			ResourceManager::LoadTexture("models/backpack/normal.png", Texture::Type::normal, GL_REPEAT, false),
			ResourceManager::LoadTexture("models/backpack/specular.jpg", Texture::Type::specular, GL_REPEAT, false),
	});
	backpack->setRenderer(new ModelRenderer(&backpackModel, backpackMaterial));
	backpack->transform.position = { -5.f, -1.f, 0.f };

	testRedBoxModel = Model("models/normalBoxTest/Box_normal_example.obj");
	Material* testRedBoxMaterial = ResourceManager::LoadMaterial("testRedBox", litNormalShader);
	testRedBoxMaterial->AddTextures(std::vector<Texture*>{
		ResourceManager::LoadTexture("models/normalBoxTest/box_example_None_BaseColor.png", Texture::Type::diffuse, GL_REPEAT, true),
			ResourceManager::LoadTexture("models/normalBoxTest/box_example_None_Normal.png", Texture::Type::normal, GL_REPEAT, true),
	});
	testRedBox->setRenderer(new ModelRenderer(&testRedBoxModel, testRedBoxMaterial));
	testRedBox->transform.position = { 5.f, -3.f, 2.f };

	soulSpearModel = Model(std::string("models/soulspear/soulspear.obj"), true);
	Material* soulSpearMaterial = ResourceManager::LoadMaterial("soulSpear", litNormalShader);
	soulSpearMaterial->AddTextures(std::vector<Texture*>{
		ResourceManager::LoadTexture("models/soulspear/soulspear_diffuse.tga", Texture::Type::diffuse, GL_REPEAT, true),
			ResourceManager::LoadTexture("models/soulspear/soulspear_specular.tga", Texture::Type::specular, GL_REPEAT, true),
			ResourceManager::LoadTexture("models/soulspear/soulspear_normal.tga", Texture::Type::normal, GL_REPEAT, true),
	});
	soulSpear->setRenderer(new ModelRenderer(&soulSpearModel, soulSpearMaterial));
	soulSpear->transform.position = { 5.f, 1.f, 1.f };

	puppetModel.LoadModel(std::string("models/Character.fbx"));
	Material* puppetMaterial = ResourceManager::LoadMaterial("puppet", animateShader);
	puppetMaterial->AddTextures(std::vector<Texture*> {
		ResourceManager::LoadTexture("images/puppet/DummyBaseMap.tga", Texture::Type::diffuse),
			ResourceManager::LoadTexture("images/puppet/DummyNormalMap.tga", Texture::Type::normal),
	});
	puppet->transform.position.y -= 4;
	puppet->transform.scale = 0.01f;
	puppet->setRenderer(new ModelRenderer(&puppetModel, puppetMaterial));

	xbotModel.LoadModel(std::string("models/X Bot.fbx"));
	xbot->transform.scale = 0.01f;
	xbot->transform.position = { 0.f, -0.5f, 1.5f };
	Material* xbotMaterial = ResourceManager::LoadMaterial("puppet", animateShader);
	xbotMaterial->AddTextures(std::vector<Texture*> {
		ResourceManager::LoadTexture("models/soulspear/soulspear_diffuse.tga", Texture::Type::diffuse)
	});
	xbot->setRenderer(new ModelRenderer(&xbotModel, xbotMaterial));

	xbotChicken = Animation("models/Idle.fbx", &xbotModel);
	xbotAnimator = Animator(&xbotChicken);

	vampireModel.LoadModel(std::string("models/dancing_vampire.dae"));
	//vampire->transform.scale = 0.01;
	//vampire->transform.position = { 0.f, -0.5f, 1.f };
	Material* vampireMaterial = ResourceManager::LoadMaterial("vampire", animateShader);
	vampireMaterial->AddTextures(std::vector<Texture*>{
		ResourceManager::LoadTexture("models/Vampire_diffuse.png", Texture::Type::diffuse)
	});
	vampire->setRenderer(new ModelRenderer(&vampireModel, vampireMaterial));

	vampireWalk = Animation("models/dancing_vampire.dae", &vampireModel);
	vampireAnimator = Animator(&vampireWalk);

	puppetAnimation = Animation("models/Character@LPunch4.fbx", &puppetModel);
	puppetAnimator = Animator(&puppetAnimation);
}

void TestScene::Update(float delta)
{
	xbotAnimator.UpdateAnimation(delta);
	vampireAnimator.UpdateAnimation(delta);
	puppetAnimator.UpdateAnimation(delta);

	messengerInterface.Update();

	pointLights[0].position.x = 1.5f * sin((float)glfwGetTime() * 2.f);
	lightCube->transform.position = pointLights[0].position;
	spotlight.position = camera->position;
	spotlight.direction = camera->front;
	
	// Different View Projection matrix for the skybox, as translations shouldn't affect it
	glm::mat4 skyBoxView = glm::mat4(glm::mat3(camera->GetViewMatrix()));
	glm::mat4 skyboxProjection = glm::perspective(glm::radians(camera->fov), (float)*windowWidth / (float)*windowHeight, 0.01f, 100.0f);
	glm::mat4 skyBoxVP = skyboxProjection * skyBoxView;
	skybox->Update(skyBoxVP);

	for (auto i = sceneObjects.begin(); i != sceneObjects.end(); i++)
	{
		(*i)->Update(delta);
	}

	animateShader->Use();
	animateShader->setMat4("projection", glm::perspective(glm::radians(camera->fov), (float)*windowWidth / (float)*windowHeight, 0.01f, 100.0f));
	animateShader->setMat4("view", camera->GetViewMatrix());

	auto& xBotTransforms = xbotAnimator.getFinalBoneMatrices();
	for (int i = 0; i < xBotTransforms.size(); i++) {
		animateShader->setMat4("boneMatrices[" + std::to_string(i) + "]", xBotTransforms[i]);
	}
	animateShader->setMat4("model", xbot->transform.getGlobalMatrix());
	xbot->Draw();

	auto& vampTransforms = vampireAnimator.getFinalBoneMatrices();
	for (int i = 0; i < vampTransforms.size(); i++) {
		animateShader->setMat4("boneMatrices[" + std::to_string(i) + "]", vampTransforms[i]);
	}
	animateShader->setMat4("model", vampire->transform.getGlobalMatrix());
	vampire->Draw();

	auto& puppetTransforms = puppetAnimator.getFinalBoneMatrices();
	for (int i = 0; i < vampTransforms.size(); i++) {
		animateShader->setMat4("boneMatrices[" + std::to_string(i) + "]", puppetTransforms[i]);
	}
	animateShader->setMat4("model", puppet->transform.getGlobalMatrix());
	puppet->Draw();
}

void TestScene::Draw()
{
	// TODO: Actual draw/update loop
	for (auto i = sceneObjects.begin(); i != sceneObjects.end(); i++)
	{
		(*i)->Draw();
	}
	skybox->Draw();
}

void TestScene::GUI()
{
	if (!ImGui::Begin("Network Thing", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::End();
		return;
	}
	messengerInterface.GUI();
	ImGui::End();
}

TestScene::~TestScene()
{
}
