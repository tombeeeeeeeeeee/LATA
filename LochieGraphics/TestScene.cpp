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
	Shader* animateShader = ResourceManager::LoadShader("shaders/animate.vert", "shaders/animate.frag", Shader::Flags::Animated);
	Shader* pbrShader = ResourceManager::LoadShader("shaders/pbr.vert", "shaders/pbr.frag", Shader::Flags::Lit | Shader::Flags::VPmatrix);
	screenShader = ResourceManager::LoadShader("shaders/framebuffer.vert", "shaders/framebuffer.frag");
	shadowMapDepth = ResourceManager::LoadShader("shaders/simpleDepthShader.vert", "shaders/simpleDepthShader.frag");
	shadowMapping = ResourceManager::LoadShader("shaders/shadowMapping.vert", "shaders/shadowMapping.frag", Shader::Flags::Lit | Shader::Flags::VPmatrix);
	shadowDebug = ResourceManager::LoadShader("shaders/shadowDebug.vert", "shaders/shadowDebug.frag");
	Shader* simpleTextured = ResourceManager::LoadShader("shaders/simpleTextured.vert", "shaders/simpleTextured.frag", Shader::Flags::VPmatrix);
	superShader = ResourceManager::LoadShader("shaders/superShader.vert", "shaders/superShader.frag", Shader::Flags::Lit | Shader::Flags::VPmatrix);
	uiShader = ResourceManager::LoadShader("shaders/ui.vert", "shaders/ui.frag");
	hdrBloom = ResourceManager::LoadShaderDefaultVert("shaders/HDRBloom.frag");

	shaders = std::vector<Shader*>{ litNormalShader, litShader, lightCubeShader, skyBoxShader, animateShader, pbrShader, screenShader, shadowMapDepth, shadowMapping, shadowDebug,
		simpleTextured, superShader, simpleTextured,
	};


	// TODO: This needs to be cleaned up
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
	Material* backpackMaterial = ResourceManager::LoadMaterial("backpack", superShader);
	backpackMaterial->AddTextures(std::vector<Texture*>{
		ResourceManager::LoadTexture("models/backpack/diffuse.jpg", Texture::Type::albedo, GL_REPEAT, false),
			ResourceManager::LoadTexture("models/backpack/normal.png", Texture::Type::normal, GL_REPEAT, false),
			ResourceManager::LoadTexture("models/backpack/specular.jpg", Texture::Type::metallic, GL_REPEAT, false),
			ResourceManager::LoadTexture("models/backpack/roughness.jpg", Texture::Type::roughness, GL_REPEAT, false),
	});
	backpack->setRenderer(new ModelRenderer(&backpackModel, backpackMaterial));
	backpack->transform.position = { -4.5f, 1.7f, 0.f };
	backpack->transform.setEulerRotation({0.f, 52.6f, 0.f});
	
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
	Material* testRedBoxMaterial = ResourceManager::LoadMaterial("testRedBox", superShader);
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

	xbotModel.LoadModel(std::string("models/X Bot.fbx"));
	xbot->transform.scale = 0.01f;
	xbot->transform.position = { 0.f, -0.5f, 1.5f };
	Material* xbotMaterial = ResourceManager::LoadMaterial("puppet", superShader);
	xbotMaterial->AddTextures(std::vector<Texture*> {
		ResourceManager::LoadTexture("models/soulspear/soulspear_diffuse.tga", Texture::Type::diffuse)
	});
	xbot->setRenderer(new ModelRenderer(&xbotModel, xbotMaterial));

	xbotChicken = Animation("models/Chicken Dance.fbx", &xbotModel);
	xbotIdle = Animation("models/Ymca Dance.fbx", &xbotModel);
	xbotAnimator = Animator(&xbotChicken);
	xbotOtherAnimator = Animator(&xbotIdle);
	xbotBlendedAnimator = BlendedAnimator(&xbotChicken, &xbotIdle);

	vampireModel.LoadModel(std::string("models/Skinning Test.fbx"));
	vampire->transform.scale = 0.01f;
	//vampire->transform.position = { 0.f, -0.5f, 1.f };
	Material* vampireMaterial = ResourceManager::LoadMaterial("vampire", superShader);
	vampireMaterial->AddTextures(std::vector<Texture*>{
		ResourceManager::LoadTexture("models/Vampire_diffuse.png", Texture::Type::albedo),
			ResourceManager::LoadTexture("models/Vampire_normal.png", Texture::Type::normal)
	});
	vampire->setRenderer(new ModelRenderer(&vampireModel, vampireMaterial));
	vampire->transform.position = { 1.6f, -0.5f, -2.f };


	vampireWalk = Animation("models/Skinning Test.fbx", &vampireModel);
	vampireAnimator = Animator(&vampireWalk);

	shadowDebugQuad.InitialiseQuad(0.5f, 0.5f);
	screenQuad.InitialiseQuad(1.f, 0.0f);
	buttonQuad.InitialiseQuad(0.25f, 0.f);

	buttonTexture = ResourceManager::LoadTexture("images/thisOneMightBeIt.png", Texture::Type::albedo, GL_CLAMP_TO_EDGE, true);
	//buttonTexture = ResourceManager::LoadTexture("images/testButtonWorking.png", Texture::Type::albedo, GL_CLAMP_TO_EDGE, true);

	// Create colour attachment texture for fullscreen framebuffer
	screenColourBuffer = ResourceManager::LoadTexture(*windowWidth, *windowHeight, GL_RGB, nullptr, GL_CLAMP_TO_EDGE, GL_UNSIGNED_BYTE, false, GL_LINEAR, GL_LINEAR);
	
	// Make fullscreen framebuffer
	screenFrameBuffer = new FrameBuffer(*windowWidth, *windowHeight, screenColourBuffer, nullptr, true);

	// TODO: Should be done for each light
	// Create shadow depth texture for the light
	depthMap = ResourceManager::LoadTexture(directionalLight.shadowTexWidth, directionalLight.shadowTexHeight, GL_DEPTH_COMPONENT, nullptr, GL_CLAMP_TO_BORDER, GL_FLOAT, false, GL_NEAREST, GL_NEAREST);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 }; // TODO: Move to be apart of texture
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	shadowFrameBuffer = new FrameBuffer(directionalLight.shadowTexWidth, directionalLight.shadowTexHeight, nullptr, depthMap, false);
	shadowDebug->Use();
	shadowDebug->setInt("depthMap", 1);
}

void TestScene::EarlyUpdate()
{
}

void TestScene::Update(float delta)
{
	if (animateBlendDirectionRight) {
		xbotBlendedAnimator.lerpAmount += delta;
		if (xbotBlendedAnimator.lerpAmount > 1.f) { xbotBlendedAnimator.lerpAmount = 1.f; }
	}
	else
	{
		xbotBlendedAnimator.lerpAmount -= delta;
		if (xbotBlendedAnimator.lerpAmount < 0) { xbotBlendedAnimator.lerpAmount = 0; }
	}

	// TODO: rather then constanty reloading the framebuffer, the texture could link to the framebuffers that need assoisiate with it? or maybe just refresh all framebuffers when a texture is loaded?
	shadowFrameBuffer->Load();

	messengerInterface.Update();

	pointLights[0].position.x = 1.5f * sinf((float)glfwGetTime() * 2.f);
	lightCube->transform.position = pointLights[0].position;
	spotlight.position = camera->position;
	spotlight.direction = camera->front;
	
	// TODO: Skybox class exists, could try to work out the vp there
	// Different View Projection matrix for the skybox, as translations shouldn't affect it
	// TODO: Would like to make this below function more clear, maybe serperate into like a remove translation function or something
	glm::mat4 skyBoxView = glm::mat4(glm::mat3(camera->GetViewMatrix()));
	// TODO: This math shouldn't be here, maybe move to camera class or get the projection from scenemanager
	glm::mat4 skyboxProjection = glm::perspective(glm::radians(camera->fov), (float)*windowWidth / (float)*windowHeight, camera->nearPlane, camera->farPlane);
	glm::mat4 skyBoxVP = skyboxProjection * skyBoxView;
	skybox->Update(skyBoxVP);

	for (auto i = sceneObjects.begin(); i != sceneObjects.end(); i++)
	{
		(*i)->Update(delta);
	}

	xbotAnimator.UpdateAnimation(delta);
	xbotOtherAnimator.UpdateAnimation(delta);
	xbotBlendedAnimator.UpdateAnimation(delta);
	vampireAnimator.UpdateAnimation(delta);
}

void TestScene::Draw()
{
	auto& xBotTransforms = xbotAnimator.getFinalBoneMatrices();
	auto& xBotOtherTransforms = xbotOtherAnimator.getFinalBoneMatrices();
	auto& vampTransforms = vampireAnimator.getFinalBoneMatrices();

	auto& xbotInterpolatedAnimations = xbotBlendedAnimator.getFinalBoneMatrices();


	
	//glCullFace(GL_FRONT);
	//RENDER SCENE
	for (auto i = sceneObjects.begin(); i != sceneObjects.end(); i++)
	{
		ModelRenderer* currentRenderer = (*i)->getRenderer();
		if (currentRenderer) {
			Texture* alphaMap = currentRenderer->material->getFirstTextureOfType(Texture::Type::diffuse);
			if (!alphaMap) {
				alphaMap = currentRenderer->material->getFirstTextureOfType(Texture::Type::albedo);
			}
			if (alphaMap) {
				// TODO: Really should be using a Texture bind function here.
				alphaMap->Bind(1);
				shadowMapDepth->setSampler("alphaDiscardMap", 1);
			}
			else {
				// TODO:
				shadowMapDepth->setSampler("alphaDiscardMap", 0); 
			}
		}
		(*i)->Draw(shadowMapDepth);
	}


	for (int i = 0; i < xbotInterpolatedAnimations.size(); i++) {
		shadowMapDepth->setMat4("boneMatrices[" + std::to_string(i) + "]", xbotInterpolatedAnimations[i]);
	}
	shadowMapDepth->setMat4("model", xbot->transform.getGlobalMatrix());
	xbot->Draw(shadowMapDepth);

	for (int i = 0; i < vampTransforms.size(); i++) {
		shadowMapDepth->setMat4("boneMatrices[" + std::to_string(i) + "]", vampTransforms[i]);
	}
	shadowMapDepth->setMat4("model", vampire->transform.getGlobalMatrix());
	vampire->Draw(shadowMapDepth);

	skybox->Draw();
	//glCullFace(GL_BACK);

	// Render scene with shadow map, to the screen framebuffer
	screenFrameBuffer->Bind();

	// TODO: move viewport changing stuff into FrameBuffer
	glViewport(0, 0, *windowWidth, *windowHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shadowMapping->Use();

	// set light uniforms
	shadowMapping->setVec3("viewPos", camera->position);
	shadowMapping->setVec3("lightPos", light->getPos());
	shadowMapping->setMat4("lightSpaceMatrix", lightSpaceMatrix);

	depthMap->Bind(17);
	shadowMapping->setSampler("shadowMap", 17);
	
	superShader->Use();

	// set light uniforms
	// TODO: Shouldn't need to set light uniforms here, use the shader flags and make one for shadowed
	superShader->setVec3("viewPos", camera->position);
	superShader->setVec3("lightPos", light->getPos());
	superShader->setMat4("directionalLightSpaceMatrix", lightSpaceMatrix);

	superShader->setSampler("shadowMap", 17);
	depthMap->Bind(17);

	// RENDER SCENE
	for (auto i = sceneObjects.begin(); i != sceneObjects.end(); i++)
	{
		(*i)->Draw();
	}
	skybox->Draw();

	// Draw animated stuff
	superShader->Use();
	glm::mat4 projection = glm::perspective(glm::radians(camera->fov), (float)*windowWidth / (float)*windowHeight, camera->nearPlane, camera->farPlane);
	superShader->setMat4("vp", projection * camera->GetViewMatrix());

	for (int i = 0; i < xbotInterpolatedAnimations.size(); i++) {
		superShader->setMat4("boneMatrices[" + std::to_string(i) + "]", xbotInterpolatedAnimations[i]);
	}
	superShader->setMat4("model", xbot->transform.getGlobalMatrix());
	xbot->Draw();

	for (int i = 0; i < vampTransforms.size(); i++) {
		superShader->setMat4("boneMatrices[" + std::to_string(i) + "]", vampTransforms[i]);
	}
	superShader->setMat4("model", vampire->transform.getGlobalMatrix());
	vampire->Draw();

	if (showShadowDebug) {
		// Debug render the light depth map
		shadowDebug->Use();
		shadowDebug->setFloat("near_plane", light->shadowNearPlane);
		shadowDebug->setFloat("far_plane", light->shadowFarPlane);
		depthMap->Bind(1);
		shadowDebugQuad.Draw();
	}


	// Unbind framebuffer
	FrameBuffer::Unbind();
	glDisable(GL_DEPTH_TEST); // Disable depth test for fullscreen quad

	screenShader->Use();
	screenColourBuffer->Bind(1);
	screenShader->setSampler("screenTexture", 1);
	screenQuad.Draw();

	if (showButton) {
		//screenShader->Use();
		//screenShader->setSampler("screenTexture", 1);
		uiShader->Use();
		buttonTexture->Bind(1);
		uiShader->setSampler("image", 1);
		buttonQuad.Draw();
	}

	// Re enable the depth test
	glEnable(GL_DEPTH_TEST);
}

void TestScene::OnMouseDown()
{
	// TODO: Make a button class
	if (cursorPos->x > 0.375f && cursorPos->x < 0.625f && cursorPos->y > 0.375f && cursorPos->y < 0.625f)
	{
		std::cout << "Button pressed!\n";
		animateBlendDirectionRight = !animateBlendDirectionRight;
		// Cursor is hovered on button
		//if ()
	}
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
		ImGui::SliderFloat("Animation trans", &xbotBlendedAnimator.lerpAmount, 0.f, 1.0f);
		ImGui::Checkbox("Show shadow debug", &showShadowDebug);
		ImGui::Checkbox("Show the red button that toggles animation", &showButton);
		ImGui::End();
	}
}

void TestScene::OnWindowResize()
{
	//glDeleteTextures(1, &textureColorbuffer);

	screenColourBuffer->setWidthHeight((int)*windowWidth, (int)*windowHeight);
	screenFrameBuffer->setWidthHeight(*windowWidth, *windowHeight);
}

TestScene::~TestScene()
{
	// TODO: Not deleting the textures properly
	//glDeleteTextures(1, &textureColorbuffer);
	delete shadowFrameBuffer;
	delete screenFrameBuffer;
}
