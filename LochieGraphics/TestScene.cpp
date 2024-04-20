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
	pbrShader = ResourceManager::LoadShader("shaders/pbr.vert", "shaders/pbr.frag", Shader::Flags::Lit | Shader::Flags::VPmatrix);
	screenShader = ResourceManager::LoadShader("shaders/framebuffer.vert", "shaders/framebuffer.frag");
	shadowMapDepth = ResourceManager::LoadShader("shaders/simpleDepthShader.vert", "shaders/simpleDepthShader.frag");
	shadowMapping = ResourceManager::LoadShader("shaders/shadowMapping.vert", "shaders/shadowMapping.frag", Shader::Flags::Lit | Shader::Flags::VPmatrix);

	shaders = std::vector<Shader*>{ litNormalShader, litShader, lightCubeShader, skyBoxShader, animateShader, pbrShader, screenShader, shadowMapDepth, shadowMapping };


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


	Material* boxMaterial = ResourceManager::LoadMaterial("box", shadowMapping);
	boxMaterial->AddTextures(std::vector<Texture*> {
		ResourceManager::LoadTexture("images/container2.png", Texture::Type::diffuse),
			ResourceManager::LoadTexture("images/container2_specular.png", Texture::Type::specular),
	});
	cubeModel.AddMesh(new Mesh(Mesh::presets::cube));
	boxes->setRenderer(new ModelRenderer(&cubeModel, boxMaterial));
	boxes->transform.scale = 30.0f;
	boxes->transform.position = { 0.f , -20.f, 0.f };

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
	Material* testRedBoxMaterial = ResourceManager::LoadMaterial("testRedBox", pbrShader);
	testRedBoxMaterial->AddTextures(std::vector<Texture*>{
		ResourceManager::LoadTexture("models/normalBoxTest/box_example_None_BaseColor.png", Texture::Type::albedo, GL_REPEAT, true),
			ResourceManager::LoadTexture("models/normalBoxTest/box_example_None_Normal.png", Texture::Type::normal, GL_REPEAT, true),
			ResourceManager::LoadTexture("models/normalBoxTest/box_example_None_Metallic.png", Texture::Type::metallic, GL_REPEAT, true),
			//ResourceManager::LoadTexture("models/normalBoxTest/box_example_None_AO.png", Texture::Type::ao, GL_REPEAT, true),
			ResourceManager::LoadTexture("models/normalBoxTest/box_example_None_Roughness.png", Texture::Type::roughness, GL_REPEAT, true),
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

	screenQuad.InitialiseQuad(1.0f);

	LoadRenderBuffer();

	glGenFramebuffers(1, &depthMapFBO);
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	//glDrawBuffer(GL_NONE);
	//glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void TestScene::EarlyUpdate()
{
	//// bind to framebuffer and draw scene as we normally would to color texture 
	//glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)
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

	glEnable(GL_DEPTH_TEST);

	glDepthFunc(GL_LESS);



	// 1. render depth of scene to texture (from light's perspective)
		// --------------------------------------------------------------
	glm::mat4 lightProjection, lightView;
	glm::mat4 lightSpaceMatrix;
	float near_plane = 1.0f;
	float far_plane = 70.5f;
	lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	lightView = glm::lookAt(-20.f * directionalLight.direction, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
	lightSpaceMatrix = lightProjection * lightView;
	// render scene from light's point of view
	shadowMapDepth->Use();
	shadowMapDepth->setMat4("lightSpaceMatrix", lightSpaceMatrix);

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	shadowMapping->Use();
	shadowMapping->setSampler("shadowMap", 15);
	glActiveTexture(GL_TEXTURE0 + 15);
	glBindTexture(GL_TEXTURE_2D, depthMap);

	for (auto i = sceneObjects.begin(); i != sceneObjects.end(); i++)
	{
		(*i)->Draw();
	}
	skybox->Draw();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// reset viewport
	glViewport(0, 0, *windowWidth, *windowHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 2. render scene as normal using the generated depth/shadow map  
		// --------------------------------------------------------------
	shadowMapping->Use();
	glm::mat4 projection = glm::perspective(glm::radians(camera->fov), (float)*windowWidth / (float)*windowHeight, 0.1f, 1000.0f);
	glm::mat4 view = camera->GetViewMatrix();
	shadowMapping->setMat4("vp", projection * view);
	// set light uniforms
	shadowMapping->setVec3("viewPos", camera->position);
	shadowMapping->setVec3("lightPos", -20.f * directionalLight.direction);
	shadowMapping->setMat4("lightSpaceMatrix", lightSpaceMatrix);
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, depthMap);



	// TODO: Actual draw/update loop
	for (auto i = sceneObjects.begin(); i != sceneObjects.end(); i++)
	{
		(*i)->Draw();
	}
	skybox->Draw();

	//// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.

	//screenShader->Use();
	////glBindVertexArray(screenQuad);
	//glActiveTexture(GL_TEXTURE0 + 1);
	//screenShader->setSampler("screenTexture", 1);
	//glBindTexture(GL_TEXTURE_2D, textureColorbuffer);	// use the color attachment texture as the texture of the quad plane
	//screenQuad.Draw();
	////glDrawArrays(GL_TRIANGLES, 0, 6);
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

void TestScene::OnWindowResize()
{
	glDeleteRenderbuffers(1, &rbo);
	glDeleteFramebuffers(1, &framebuffer);
	glDeleteTextures(1, &textureColorbuffer);


	LoadRenderBuffer();
}

TestScene::~TestScene()
{
	glDeleteRenderbuffers(1, &rbo);
	glDeleteFramebuffers(1, &framebuffer);
	glDeleteTextures(1, &textureColorbuffer);

}

void TestScene::LoadRenderBuffer()
{
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	// create a color attachment texture
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, *windowWidth, *windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
	// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	// use a single renderbuffer object for both a depth AND stencil buffer.
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, *windowWidth, *windowHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
	// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << "\n";
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
