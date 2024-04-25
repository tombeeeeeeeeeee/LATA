#include "TestScene.h"

#include "stb_image.h"

#include <iostream>
#include <array>

TestScene::TestScene()
{
	sceneObjects = std::vector<SceneObject*>{
		boxes,
		grass,
		lightCube,
		backpack,
		soulSpear,
		testRedBox,
		tires,
		//bottle
		//puppet,
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
	Shader* litNormalShader = ResourceManager::LoadShader("shaders/litNormal.vert", "shaders/litNormal.frag", Shader::Flags::Lit | Shader::Flags::VPmatrix);
	Shader* litShader = ResourceManager::LoadShader("shaders/lit.vert", "shaders/lit.frag", Shader::Flags::Lit | Shader::Flags::VPmatrix);
	Shader* lightCubeShader = ResourceManager::LoadShader("shaders/lightCube.vert", "shaders/lightCube.frag", Shader::Flags::VPmatrix);
	Shader* skyBoxShader = ResourceManager::LoadShader("shaders/cubemap.vert", "shaders/cubemap.frag");
	animateShader = ResourceManager::LoadShader("shaders/animate.vert", "shaders/animate.frag", Shader::Flags::Animated);
	Shader* pbrShader = ResourceManager::LoadShader("shaders/pbr.vert", "shaders/pbr.frag", Shader::Flags::Lit | Shader::Flags::VPmatrix);
	screenShader = ResourceManager::LoadShader("shaders/framebuffer.vert", "shaders/framebuffer.frag");
	shadowMapDepth = ResourceManager::LoadShader("shaders/simpleDepthShader.vert", "shaders/simpleDepthShader.frag");
	shadowMapping = ResourceManager::LoadShader("shaders/shadowMapping.vert", "shaders/shadowMapping.frag", Shader::Flags::Lit | Shader::Flags::VPmatrix);
	shadowDebug = ResourceManager::LoadShader("shaders/shadowDebug.vert", "shaders/shadowDebug.frag");
	

	shaders = std::vector<Shader*>{ litNormalShader, litShader, lightCubeShader, skyBoxShader, animateShader, pbrShader, screenShader, shadowMapDepth, shadowMapping, shadowDebug,
		ResourceManager::LoadShader("shaders/simpleTextured.vert", "shaders/simpleTextured.frag", Shader::Flags::VPmatrix),
	};


	// TODO: This
	std::array<std::string, 6> skyboxFaces;
	skyboxFaces = { "images/skybox/right.jpg", "images/skybox/left.jpg", "images/skybox/top.jpg", "images/skybox/bottom.jpg", "images/skybox/front.jpg", "images/skybox/back.jpg" };
	skyboxes.push_back(new Skybox(skyBoxShader, Texture::LoadCubeMap(skyboxFaces.data())));
	skyboxFaces = { "images/otherskybox/right.png", "images/otherskybox/left.png", "images/otherskybox/top.png", "images/otherskybox/bottom.png", "images/otherskybox/front.png", "images/otherskybox/back.png" };
	skyboxes.push_back(new Skybox(skyBoxShader, Texture::LoadCubeMap(skyboxFaces.data())));
	skyboxFaces = { "images/SkyBox Volume 2/DeepSpaceBlue/leftImage.png", "images/SkyBox Volume 2/DeepSpaceBlue/rightImage.png", "images/SkyBox Volume 2/DeepSpaceBlue/upImage.png", "images/SkyBox Volume 2/DeepSpaceBlue/downImage.png", "images/SkyBox Volume 2/DeepSpaceBlue/frontImage.png", "images/SkyBox Volume 2/DeepSpaceBlue/backImage.png" };
	skyboxes.push_back(new Skybox(skyBoxShader, Texture::LoadCubeMap(skyboxFaces.data())));
	skyboxFaces = { "images/SkyBox Volume 2/DeepSpaceBlueWithPlanet/leftImage.png", "images/SkyBox Volume 2/DeepSpaceBlueWithPlanet/rightImage.png", "images/SkyBox Volume 2/DeepSpaceBlueWithPlanet/upImage.png", "images/SkyBox Volume 2/DeepSpaceBlueWithPlanet/downImage.png", "images/SkyBox Volume 2/DeepSpaceBlueWithPlanet/frontImage.png", "images/SkyBox Volume 2/DeepSpaceBlueWithPlanet/backImage.png" };
	skyboxes.push_back(new Skybox(skyBoxShader, Texture::LoadCubeMap(skyboxFaces.data())));
	skyboxFaces = { "images/SkyBox Volume 2/DeepSpaceGreen/leftImage.png", "images/SkyBox Volume 2/DeepSpaceGreen/rightImage.png", "images/SkyBox Volume 2/DeepSpaceGreen/upImage.png", "images/SkyBox Volume 2/DeepSpaceGreen/downImage.png", "images/SkyBox Volume 2/DeepSpaceGreen/frontImage.png", "images/SkyBox Volume 2/DeepSpaceGreen/backImage.png" };
	skyboxes.push_back(new Skybox(skyBoxShader, Texture::LoadCubeMap(skyboxFaces.data())));
	skyboxFaces = { "images/SkyBox Volume 2/DeepSpaceGreenWithPlanet/leftImage.png", "images/SkyBox Volume 2/DeepSpaceGreenWithPlanet/rightImage.png", "images/SkyBox Volume 2/DeepSpaceGreenWithPlanet/upImage.png", "images/SkyBox Volume 2/DeepSpaceGreenWithPlanet/downImage.png", "images/SkyBox Volume 2/DeepSpaceGreenWithPlanet/frontImage.png", "images/SkyBox Volume 2/DeepSpaceGreenWithPlanet/backImage.png" };
	skyboxes.push_back(new Skybox(skyBoxShader, Texture::LoadCubeMap(skyboxFaces.data())));
	skyboxFaces = { "images/SkyBox Volume 2/DeepSpaceRed/leftImage.png", "images/SkyBox Volume 2/DeepSpaceRed/rightImage.png", "images/SkyBox Volume 2/DeepSpaceRed/upImage.png", "images/SkyBox Volume 2/DeepSpaceRed/downImage.png", "images/SkyBox Volume 2/DeepSpaceRed/frontImage.png", "images/SkyBox Volume 2/DeepSpaceRed/backImage.png" };
	skyboxes.push_back(new Skybox(skyBoxShader, Texture::LoadCubeMap(skyboxFaces.data())));
	skyboxFaces = { "images/SkyBox Volume 2/DeepsSpaceRedWithPlanet/leftImage.png", "images/SkyBox Volume 2/DeepsSpaceRedWithPlanet/rightImage.png", "images/SkyBox Volume 2/DeepsSpaceRedWithPlanet/upImage.png", "images/SkyBox Volume 2/DeepsSpaceRedWithPlanet/downImage.png", "images/SkyBox Volume 2/DeepsSpaceRedWithPlanet/frontImage.png", "images/SkyBox Volume 2/DeepsSpaceRedWithPlanet/backImage.png" };
	skyboxes.push_back(new Skybox(skyBoxShader, Texture::LoadCubeMap(skyboxFaces.data())));
	skyboxFaces = { "images/SkyBox Volume 2/Stars01/leftImage.png", "images/SkyBox Volume 2/Stars01/rightImage.png", "images/SkyBox Volume 2/Stars01/upImage.png", "images/SkyBox Volume 2/Stars01/downImage.png", "images/SkyBox Volume 2/Stars01/frontImage.png", "images/SkyBox Volume 2/Stars01/backImage.png" };
	skyboxes.push_back(new Skybox(skyBoxShader, Texture::LoadCubeMap(skyboxFaces.data())));
	//TODO: Should be using the resource manager
	//skybox = new Skybox(skyBoxShader, Texture::LoadCubeMap(skyboxFaces));
	skybox = skyboxes[5];
	skyboxIndex = 5;

	Material* boxMaterial = ResourceManager::LoadMaterial("box", shadowMapping);
	boxMaterial->AddTextures(std::vector<Texture*> {
		ResourceManager::LoadTexture("images/container2.png", Texture::Type::diffuse),
			ResourceManager::LoadTexture("images/container2_specular.png", Texture::Type::specular),
	});
	cubeModel.AddMesh(new Mesh(Mesh::presets::cube));
	boxes->setRenderer(new ModelRenderer(&cubeModel, boxMaterial));
	boxes->transform.scale = 30.0f;
	boxes->transform.position = { 0.f, -15.450f, 0.f };

	Material* lightCubeMaterial = ResourceManager::LoadMaterial("lightCube", lightCubeShader);
	lightCube->setRenderer(new ModelRenderer(&cubeModel, lightCubeMaterial));
	lightCube->transform.scale = 0.2f;

	Material* grassMaterial = ResourceManager::LoadMaterial("grass", litShader);
	grassMaterial->AddTextures(std::vector<Texture*>{
		ResourceManager::LoadTexture("images/grass.png", Texture::Type::diffuse, GL_CLAMP_TO_EDGE, false),
	});
	grassModel.AddMesh(new Mesh(Mesh::presets::doubleQuad));
	grass->setRenderer(new ModelRenderer(&grassModel, grassMaterial));
	grass->transform.position = { 1.9f, 0.f, 2.6f };
	grass->transform.setEulerRotation({ 0.f, -43.2f, 0.f });

	backpackModel = Model("models/backpack/backpack.obj", false);
	Material* backpackMaterial = ResourceManager::LoadMaterial("backpack", pbrShader);
	backpackMaterial->AddTextures(std::vector<Texture*>{
		ResourceManager::LoadTexture("models/backpack/diffuse.jpg", Texture::Type::albedo, GL_REPEAT, false),
			ResourceManager::LoadTexture("models/backpack/normal.png", Texture::Type::normal, GL_REPEAT, false),
			ResourceManager::LoadTexture("models/backpack/specular.jpg", Texture::Type::metallic, GL_REPEAT, false),
			ResourceManager::LoadTexture("models/backpack/roughness.jpg", Texture::Type::roughness, GL_REPEAT, false),
	});
	backpack->setRenderer(new ModelRenderer(&backpackModel, backpackMaterial));
	backpack->transform.position = { -4.5f, 1.7f, 0.f };
	backpack->transform.setEulerRotation({0.f, 52.6f, 0.f});

	bottleModel = Model("models/thermos-hydration-bottle-24oz/Thermos2.fbx", false);
	Material* bottleMaterial = ResourceManager::LoadMaterial("bottle", pbrShader);
	bottleMaterial->AddTextures(std::vector<Texture*>{
		ResourceManager::LoadTexture("models/thermos-hydration-bottle-24oz/Thermos_albedo.jpg", Texture::Type::albedo, GL_REPEAT, true),
			ResourceManager::LoadTexture("models/thermos-hydration-bottle-24oz/Thermos_normal.png", Texture::Type::normal, GL_REPEAT, true),
			ResourceManager::LoadTexture("models/thermos-hydration-bottle-24oz/Thermos_metallic.jpg", Texture::Type::metallic, GL_REPEAT, true), 
			ResourceManager::LoadTexture("models/thermos-hydration-bottle-24oz/Thermos_roughness.jpg", Texture::Type::roughness, GL_REPEAT, true),
	});
	bottle->setRenderer(new ModelRenderer(&bottleModel, bottleMaterial));

	tiresModel = Model("models/old-tires-dirt-low-poly/model.dae", false);
	Material* tiresMaterial = ResourceManager::LoadMaterial("tires", pbrShader);
	tiresMaterial->AddTextures(std::vector<Texture*>{
		ResourceManager::LoadTexture("models/old-tires-dirt-low-poly/DefaultMaterial_albedo.jpeg", Texture::Type::albedo, GL_REPEAT, true),
			ResourceManager::LoadTexture("models/old-tires-dirt-low-poly/DefaultMaterial_normal.png", Texture::Type::normal, GL_REPEAT, true),
			ResourceManager::LoadTexture("models/old-tires-dirt-low-poly/DefaultMaterial_metallic.jpeg", Texture::Type::metallic, GL_REPEAT, true),
			ResourceManager::LoadTexture("models/old-tires-dirt-low-poly/DefaultMaterial_roughness.jpeg", Texture::Type::roughness, GL_REPEAT, true)
	});
	tires->setRenderer(new ModelRenderer(&tiresModel, tiresMaterial));
	tires->transform.position = {-0.1f, 0.f, 1.2f};

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
	testRedBox->transform.position = { 0.6f, 3.5f, -3.5f };
	testRedBox->transform.setEulerRotation({ 4.3f, -17.2f, -69.5f});

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

	xbotChicken = Animation("models/Chicken Dance.fbx", &xbotModel);
	xbotAnimator = Animator(&xbotChicken);

	vampireModel.LoadModel(std::string("models/Skinning Test.fbx"));
	vampire->transform.scale = 0.01f;
	//vampire->transform.position = { 0.f, -0.5f, 1.f };
	Material* vampireMaterial = ResourceManager::LoadMaterial("vampire", animateShader);
	vampireMaterial->AddTextures(std::vector<Texture*>{
		ResourceManager::LoadTexture("models/Vampire_diffuse.png", Texture::Type::diffuse)
	});
	vampire->setRenderer(new ModelRenderer(&vampireModel, vampireMaterial));
	vampire->transform.position = { 1.6f, -0.5f, -2.f };


	vampireWalk = Animation("models/Skinning Test.fbx", &vampireModel);
	vampireAnimator = Animator(&vampireWalk);

	puppetAnimation = Animation("models/Character@LPunch4.fbx", &puppetModel);
	puppetAnimator = Animator(&puppetAnimation);

	shadowDebugQuad.InitialiseQuad(0.5f, 0.5f);
	screenQuad.InitialiseQuad(1.f, 0.0f);


	// create a color attachment texture
	screenColourBuffer = ResourceManager::LoadTexture(*windowWidth, *windowHeight, GL_RGB, nullptr, GL_CLAMP_TO_EDGE, GL_UNSIGNED_BYTE, false, GL_LINEAR, GL_LINEAR);

	
	screenFrameBuffer = new FrameBuffer(*windowWidth, *windowHeight, screenColourBuffer, nullptr, true);
	screenFrameBuffer->Bind();
	

	FrameBuffer::Unbind();


	// create shadow depth texture
	depthMap = ResourceManager::LoadTexture(directionalLight.shadowTexWidth, directionalLight.shadowTexHeight, GL_DEPTH_COMPONENT, nullptr, GL_CLAMP_TO_BORDER, GL_FLOAT, false, GL_NEAREST, GL_NEAREST);
	shadowFrameBuffer = new FrameBuffer(directionalLight.shadowTexWidth, directionalLight.shadowTexHeight, nullptr, depthMap, false);
	shadowDebug->Use();
	shadowDebug->setInt("depthMap", 0);
}

void TestScene::EarlyUpdate()
{
	//// bind to framebuffer and draw scene as we normally would to color texture 
}

void TestScene::Update(float delta)
{
	// TODO: rather then constanty reloading the framebuffer, the texture could link to the framebuffers that need assoisiate with it? or maybe just refresh all framebuffers when a texture is loaded?
	shadowFrameBuffer->Load();

	messengerInterface.Update();

	pointLights[0].position.x = 1.5f * sin((float)glfwGetTime() * 2.f);
	lightCube->transform.position = pointLights[0].position;
	spotlight.position = camera->position;
	spotlight.direction = camera->front;
	
	// Different View Projection matrix for the skybox, as translations shouldn't affect it
	glm::mat4 skyBoxView = glm::mat4(glm::mat3(camera->GetViewMatrix()));
	glm::mat4 skyboxProjection = glm::perspective(glm::radians(camera->fov), (float)*windowWidth / (float)*windowHeight, camera->nearPlane, camera->farPlane);
	glm::mat4 skyBoxVP = skyboxProjection * skyBoxView;
	skybox->Update(skyBoxVP);

	for (auto i = sceneObjects.begin(); i != sceneObjects.end(); i++)
	{
		(*i)->Update(delta);
	}

	xbotAnimator.UpdateAnimation(delta);
	vampireAnimator.UpdateAnimation(delta);
	puppetAnimator.UpdateAnimation(delta);

}

void TestScene::Draw()
{
	glEnable(GL_DEPTH_TEST);

	// Render depth of scene to texture (from light's perspective)
	glm::mat4 lightSpaceMatrix;
	Light* light = &pointLights[0];
	lightSpaceMatrix = light->getShadowViewProjection();
	shadowMapDepth->Use();
	shadowMapDepth->setMat4("lightSpaceMatrix", lightSpaceMatrix);

	glViewport(0, 0, light->shadowTexWidth, light->shadowTexHeight);
	shadowFrameBuffer->Bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	
	//glCullFace(GL_FRONT);
	//RENDER SCENE
	for (auto i = sceneObjects.begin(); i != sceneObjects.end(); i++)
	{
		(*i)->Draw(shadowMapDepth); // TODO: Make the shadow map depth support animated
	}
	skybox->Draw();
	//glCullFace(GL_BACK);

	// Render scene with shadow map, to the screen framebuffer
	screenFrameBuffer->Bind();
	glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

	// TODO: move viewport changing stuff into FrameBuffer
	glViewport(0, 0, *windowWidth, *windowHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shadowMapping->Use();

	// set light uniforms
	shadowMapping->setVec3("viewPos", camera->position);
	shadowMapping->setVec3("lightPos", light->getPos());
	shadowMapping->setMat4("lightSpaceMatrix", lightSpaceMatrix);

	shadowMapping->setSampler("shadowMap", 17);
	glActiveTexture(GL_TEXTURE17);
	glBindTexture(GL_TEXTURE_2D, depthMap->GLID);
	
	// RENDER SCENE
	for (auto i = sceneObjects.begin(); i != sceneObjects.end(); i++)
	{
		(*i)->Draw();
	}
	skybox->Draw();

	// Draw animated stuff
	animateShader->Use();
	animateShader->setMat4("projection", glm::perspective(glm::radians(camera->fov), (float)*windowWidth / (float)*windowHeight, camera->nearPlane, camera->farPlane));
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
	//puppet->Draw();



	// render Depth map to quad for visual debugging
	// ---------------------------------------------
	shadowDebug->Use();
	shadowDebug->setFloat("near_plane", light->shadowNearPlane);
	shadowDebug->setFloat("far_plane", light->shadowFarPlane);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap->GLID);
	// Uncomment this to see the light POV
	if (showShadowDebug) {
		shadowDebugQuad.Draw();
	}


	// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
	FrameBuffer::Unbind();
	glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.

	screenShader->Use();
	//glBindVertexArray(screenQuad);
	glActiveTexture(GL_TEXTURE0 + 1);
	screenShader->setSampler("screenTexture", 1);
	glBindTexture(GL_TEXTURE_2D, screenColourBuffer->GLID);	// use the color attachment texture as the texture of the quad plane
	screenQuad.Draw();
}

void TestScene::GUI()
{
	if (!ImGui::Begin("Network Thing", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::End();
	}
	else {
		messengerInterface.GUI();
		ImGui::End();
	}

	if (!ImGui::Begin("Skybox", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::End();
	}
	else {
		if (ImGui::DragInt("Skybox Index", &skyboxIndex, 0.01f, 0, (unsigned int)(skyboxes.size() - 1))) {
			skybox = skyboxes[skyboxIndex];
		}
		ImGui::End();
	}

	if (!ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::End();
	}
	else {
		ImGui::Checkbox("Show shadow debug", &showShadowDebug);
		ImGui::End();
	}
}

void TestScene::OnWindowResize()
{
	//glDeleteRenderbuffers(1, &rbo);
	//glDeleteFramebuffers(1, &framebuffer);
	//glDeleteTextures(1, &textureColorbuffer);

	screenColourBuffer->setWidthHeight((int)*windowWidth, (int)*windowHeight);
	
	screenFrameBuffer->setWidthHeight(*windowWidth, *windowHeight);

}

TestScene::~TestScene()
{
	//glDeleteRenderbuffers(1, &rbo);
	//glDeleteFramebuffers(1, &framebuffer);
	//glDeleteTextures(1, &textureColorbuffer);
	delete shadowFrameBuffer;
	delete screenFrameBuffer;
	
}
