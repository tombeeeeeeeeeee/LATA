#include "TestScene.h"

#include "SceneObject.h"
#include "Camera.h"
#include "ShaderEnum.h"
#include "Skybox.h"
#include "ResourceManager.h"

#include "EditorGUI.h"

#include "stb_image.h"

#include <iostream>
#include <array>

TestScene::TestScene() : 
	backpack   (new SceneObject(this, "backpack")),
	soulSpear  (new SceneObject(this, "soulSpear")),
	testRedBox (new SceneObject(this, "testRedBox")),
	grass      (new SceneObject(this, "grass")),
	boxes      (new SceneObject(this, "boxes")),
	lightCube  (new SceneObject(this, "lightCube")),
	xbot       (new SceneObject(this, "xbot")),
	vampire    (new SceneObject(this, "vampire")),
	tires      (new SceneObject(this, "tires")),
	aniTest    (new SceneObject(this, "ani Test"))
{
	lights = std::vector<Light*>{
		&directionalLight,
		&pointLights[0],
		&pointLights[1],
		&pointLights[2],
		&pointLights[3],
		&spotlight,
	};
}

void TestScene::Start()
{
	// Shaders
	Shader* lightCubeShader = ResourceManager::LoadShader("shaders/lightCube.vert", "shaders/lightCube.frag", Shader::Flags::VPmatrix);
	Shader* animateShader = ResourceManager::LoadShader("shaders/animate.vert", "shaders/animate.frag", Shader::Flags::Animated);
	Shader* pbrShader = ResourceManager::LoadShader("shaders/pbr.vert", "shaders/pbr.frag", Shader::Flags::Lit | Shader::Flags::VPmatrix);
	Shader* simpleTextured = ResourceManager::LoadShader("shaders/simpleTextured.vert", "shaders/simpleTextured.frag", Shader::Flags::VPmatrix);
	uiShader = ResourceManager::LoadShader("shaders/ui.vert", "shaders/ui.frag");

	shaders.insert(shaders.end(), {
		lightCubeShader,
		animateShader,
		pbrShader,
		simpleTextured,
	});
	// TODO: This needs to be cleaned up
	#pragma region Skyboxes
	std::array<std::string, 6> skyboxFaces;
	skyboxFaces = { "images/skybox/right.jpg", "images/skybox/left.jpg", "images/skybox/top.jpg", "images/skybox/bottom.jpg", "images/skybox/front.jpg", "images/skybox/back.jpg" };
	skyboxes.push_back(new Skybox(shaders[skyBoxShader], Texture::LoadCubeMap(skyboxFaces.data())));
	skyboxFaces = { "images/otherskybox/right.png", "images/otherskybox/left.png", "images/otherskybox/top.png", "images/otherskybox/bottom.png", "images/otherskybox/front.png", "images/otherskybox/back.png" };
	skyboxes.push_back(new Skybox(shaders[skyBoxShader], Texture::LoadCubeMap(skyboxFaces.data())));
	skyboxFaces = { "images/SkyBox Volume 2/DeepSpaceBlue/leftImage.png", "images/SkyBox Volume 2/DeepSpaceBlue/rightImage.png", "images/SkyBox Volume 2/DeepSpaceBlue/upImage.png", "images/SkyBox Volume 2/DeepSpaceBlue/downImage.png", "images/SkyBox Volume 2/DeepSpaceBlue/frontImage.png", "images/SkyBox Volume 2/DeepSpaceBlue/backImage.png" };
	skyboxes.push_back(new Skybox(shaders[skyBoxShader], Texture::LoadCubeMap(skyboxFaces.data())));
	skyboxFaces = { "images/SkyBox Volume 2/DeepSpaceBlueWithPlanet/leftImage.png", "images/SkyBox Volume 2/DeepSpaceBlueWithPlanet/rightImage.png", "images/SkyBox Volume 2/DeepSpaceBlueWithPlanet/upImage.png", "images/SkyBox Volume 2/DeepSpaceBlueWithPlanet/downImage.png", "images/SkyBox Volume 2/DeepSpaceBlueWithPlanet/frontImage.png", "images/SkyBox Volume 2/DeepSpaceBlueWithPlanet/backImage.png" };
	skyboxes.push_back(new Skybox(shaders[skyBoxShader], Texture::LoadCubeMap(skyboxFaces.data())));
	skyboxFaces = { "images/SkyBox Volume 2/DeepSpaceGreen/leftImage.png", "images/SkyBox Volume 2/DeepSpaceGreen/rightImage.png", "images/SkyBox Volume 2/DeepSpaceGreen/upImage.png", "images/SkyBox Volume 2/DeepSpaceGreen/downImage.png", "images/SkyBox Volume 2/DeepSpaceGreen/frontImage.png", "images/SkyBox Volume 2/DeepSpaceGreen/backImage.png" };
	skyboxes.push_back(new Skybox(shaders[skyBoxShader], Texture::LoadCubeMap(skyboxFaces.data())));
	skyboxFaces = { "images/SkyBox Volume 2/DeepSpaceGreenWithPlanet/leftImage.png", "images/SkyBox Volume 2/DeepSpaceGreenWithPlanet/rightImage.png", "images/SkyBox Volume 2/DeepSpaceGreenWithPlanet/upImage.png", "images/SkyBox Volume 2/DeepSpaceGreenWithPlanet/downImage.png", "images/SkyBox Volume 2/DeepSpaceGreenWithPlanet/frontImage.png", "images/SkyBox Volume 2/DeepSpaceGreenWithPlanet/backImage.png" };
	skyboxes.push_back(new Skybox(shaders[skyBoxShader], Texture::LoadCubeMap(skyboxFaces.data())));
	skyboxFaces = { "images/SkyBox Volume 2/DeepSpaceRed/leftImage.png", "images/SkyBox Volume 2/DeepSpaceRed/rightImage.png", "images/SkyBox Volume 2/DeepSpaceRed/upImage.png", "images/SkyBox Volume 2/DeepSpaceRed/downImage.png", "images/SkyBox Volume 2/DeepSpaceRed/frontImage.png", "images/SkyBox Volume 2/DeepSpaceRed/backImage.png" };
	skyboxes.push_back(new Skybox(shaders[skyBoxShader], Texture::LoadCubeMap(skyboxFaces.data())));
	skyboxFaces = { "images/SkyBox Volume 2/DeepsSpaceRedWithPlanet/leftImage.png", "images/SkyBox Volume 2/DeepsSpaceRedWithPlanet/rightImage.png", "images/SkyBox Volume 2/DeepsSpaceRedWithPlanet/upImage.png", "images/SkyBox Volume 2/DeepsSpaceRedWithPlanet/downImage.png", "images/SkyBox Volume 2/DeepsSpaceRedWithPlanet/frontImage.png", "images/SkyBox Volume 2/DeepsSpaceRedWithPlanet/backImage.png" };
	skyboxes.push_back(new Skybox(shaders[skyBoxShader], Texture::LoadCubeMap(skyboxFaces.data())));
	skyboxFaces = { "images/SkyBox Volume 2/Stars01/leftImage.png", "images/SkyBox Volume 2/Stars01/rightImage.png", "images/SkyBox Volume 2/Stars01/upImage.png", "images/SkyBox Volume 2/Stars01/downImage.png", "images/SkyBox Volume 2/Stars01/frontImage.png", "images/SkyBox Volume 2/Stars01/backImage.png" };
	skyboxes.push_back(new Skybox(shaders[skyBoxShader], Texture::LoadCubeMap(skyboxFaces.data())));
	skyboxFaces = { "images/otherskybox/px.png", "images/otherskybox/nx.png", "images/otherskybox/py.png", "images/otherskybox/ny.png", "images/otherskybox/pz.png", "images/otherskybox/nz.png" };
	skyboxes.push_back(new Skybox(shaders[skyBoxShader], Texture::LoadCubeMap(skyboxFaces.data())));
	//TODO: Should be using the resource manager
	//skybox = new Skybox(skyBoxShader, Texture::LoadCubeMap(skyboxFaces));
	skybox = skyboxes[9];
	skyboxIndex = 9;
	#pragma endregion // Skyboxes

	Material* boxMaterial = ResourceManager::LoadMaterial("box", shaders[super]);
	boxMaterial->AddTextures(std::vector<Texture*> {
		ResourceManager::LoadTexture("images/container2.png", Texture::Type::albedo),
			ResourceManager::LoadTexture("images/container2_specular.png", Texture::Type::PBR),
	});
	cubeModel = ResourceManager::LoadModel();
	cubeModel->AddMesh(ResourceManager::LoadMesh(Mesh::presets::cube));
	boxes->setRenderer(new ModelRenderer(cubeModel, boxMaterial));
	boxes->transform()->setScale(30.0f);
	boxes->transform()->setPosition({ 0.f, -15.450f, 0.f });

	Material* lightCubeMaterial = ResourceManager::LoadMaterial("lightCube", lightCubeShader);
	lightCube->setRenderer(new ModelRenderer(cubeModel, lightCubeMaterial));
	lightCube->transform()->setScale(0.2f);

	Material* grassMaterial = ResourceManager::LoadMaterial("grass", shaders[super]);
	grassMaterial->AddTextures(std::vector<Texture*>{
		ResourceManager::LoadTexture("images/grass.png", Texture::Type::albedo, GL_CLAMP_TO_EDGE, false),
	});
	grassModel = ResourceManager::LoadModel();
	grassModel->AddMesh(ResourceManager::LoadMesh(Mesh::presets::doubleQuad));
	grass->setRenderer(new ModelRenderer(grassModel, grassMaterial));
	grass->transform()->setPosition({ 1.9f, 0.f, 2.6f });
	grass->transform()->setEulerRotation({ 0.f, -43.2f, 0.f });

	backpackModel = ResourceManager::LoadModel("models/TomTest/Cerberus_LP.FBX");
	Material* backpackMaterial = ResourceManager::LoadMaterial("backpack", shaders[super]);
	backpackMaterial->AddTextures(std::vector<Texture*>{
		ResourceManager::LoadTexture("models/TomTest/Cerberus_A.tga", Texture::Type::albedo, GL_REPEAT),
			ResourceManager::LoadTexture("models/TomTest/Cerberus_N.tga", Texture::Type::normal, GL_REPEAT),
			ResourceManager::LoadTexture("models/TomTest/Cerberus_PBR.tga", Texture::Type::PBR, GL_REPEAT),
			//ResourceManager::LoadTexture("models/backpack/roughness.jpg", Texture::Type::emission, GL_REPEAT, false),
	});
	backpack->setRenderer(new ModelRenderer(backpackModel, backpackMaterial));
	backpack->transform()->setPosition({ -4.5f, 1.7f, 0.f });
	backpack->transform()->setScale(0.075f);
	backpack->transform()->setEulerRotation({-90.f, 0.f, 0.f});


	tiresModel = ResourceManager::LoadModel("models/old-tires-dirt-low-poly/model.dae"/*, false*/);
	Material* tiresMaterial = ResourceManager::LoadMaterial("tires", shaders[super]);
	tiresMaterial->AddTextures(std::vector<Texture*>{
		ResourceManager::LoadTexture("models/old-tires-dirt-low-poly/DefaultMaterial_albedo.jpeg", Texture::Type::albedo, GL_REPEAT, true),
			ResourceManager::LoadTexture("models/old-tires-dirt-low-poly/DefaultMaterial_normal.png", Texture::Type::normal, GL_REPEAT, true),
			ResourceManager::LoadTexture("models/old-tires-dirt-low-poly/DefaultMaterial_metallic.jpeg", Texture::Type::PBR, GL_REPEAT, true),
			ResourceManager::LoadTexture("models/old-tires-dirt-low-poly/DefaultMaterial_roughness.jpeg", Texture::Type::emission, GL_REPEAT, true)
	});
	tires->setRenderer(new ModelRenderer(tiresModel, tiresMaterial));
	tires->transform()->setPosition({-0.1f, 0.f, 1.2f});

	testRedBoxModel = ResourceManager::LoadModel("models/TomBox/cube.obj");
	Material* testRedBoxMaterial = ResourceManager::LoadMaterial("testRedBox", shaders[super]);

	testRedBoxMaterial->AddTextures(std::vector<Texture*>{
		ResourceManager::LoadTexture("models/TomBox/brickDif.png", Texture::Type::albedo, GL_REPEAT, true),
			ResourceManager::LoadTexture("models/TomBox/brickNorm.png", Texture::Type::normal, GL_REPEAT, true),
			ResourceManager::LoadTexture("models/TomBox/brickSpec.png", Texture::Type::PBR, GL_REPEAT, true),
			//ResourceManager::LoadTexture("models/normalBoxTest/box_example_None_AO.png", Texture::Type::ao, GL_REPEAT, true),
			//ResourceManager::LoadTexture("models/normalBoxTest/box_example_None_Roughness.png", Texture::Type::emission, GL_REPEAT, true),
	});
	testRedBox->setRenderer(new ModelRenderer(testRedBoxModel, testRedBoxMaterial));
	testRedBox->transform()->setPosition({ 0.6f, 3.5f, -3.5f });
	testRedBox->transform()->setEulerRotation({ 4.3f, -17.2f, -69.5f});

	soulSpearModel = ResourceManager::LoadModel(std::string("models/soulspear/soulspear.obj")/*, true*/);
	Material* soulSpearMaterial = ResourceManager::LoadMaterial("soulSpear", shaders[super]);
	soulSpearMaterial->AddTextures(std::vector<Texture*>{
		ResourceManager::LoadTexture("images/otherskybox/top.png", Texture::Type::albedo, GL_REPEAT, true),
			ResourceManager::LoadTexture("models/soulspear/soulspear_specular.tga", Texture::Type::PBR, GL_REPEAT, true),
			ResourceManager::LoadTexture("models/soulspear/soulspear_normal.tga", Texture::Type::normal, GL_REPEAT, true),
	});
	soulSpear->setRenderer(new ModelRenderer(soulSpearModel, soulSpearMaterial));
	soulSpear->transform()->setPosition({ 5.f, 1.f, 1.f });

	xbotModel = ResourceManager::LoadModel(std::string("models/X Bot.fbx"));
	xbot->transform()->setScale(0.01f);
	xbot->transform()->setPosition({ 0.f, -0.5f, 1.5f });
	Material* xbotMaterial = ResourceManager::LoadMaterial("puppet", shaders[super]);
	xbotMaterial->AddTextures(std::vector<Texture*> {
		ResourceManager::LoadTexture("models/soulspear/soulspear_diffuse.tga", Texture::Type::albedo)
	});
	//xbot->setRenderer(new ModelRenderer(xbotModel, xbotMaterial));

	xbotChicken = Animation("models/Chicken Dance.fbx", xbotModel);
	xbotIdle = Animation("models/Ymca Dance.fbx", xbotModel);
	xbotAnimator = Animator(&xbotChicken);
	xbotOtherAnimator = Animator(&xbotIdle);
	xbotBlendedAnimator = BlendedAnimator(&xbotChicken, &xbotIdle);
	//xbot->setAnimator(&xbotBlendedAnimator);

	vampireModel = ResourceManager::LoadModel(std::string("models/Skinning Test.fbx"));
	vampire->transform()->setScale(0.01f);
	//vampire->transform()->setPosition({ 0.f, -0.5f, 1.f };
	Material* vampireMaterial = ResourceManager::LoadMaterial("vampire", shaders[super]);
	vampireMaterial->AddTextures(std::vector<Texture*>{
		ResourceManager::LoadTexture("models/Vampire_diffuse.png", Texture::Type::albedo),
			ResourceManager::LoadTexture("models/Vampire_normal.png", Texture::Type::normal)
	});
	vampire->setRenderer(new ModelRenderer(vampireModel, vampireMaterial));
	vampire->transform()->setPosition({ 1.6f, -0.5f, -2.f });


	vampireWalk = Animation("models/Skinning Test.fbx", vampireModel);
	vampireAnimator = Animator(&vampireWalk);
	vampire->setAnimator(&vampireAnimator);

	//aniTest->setRenderer(new ModelRenderer(ResourceManager::LoadModel("C:/Users/s220518/Downloads/test/Anim_Sync_RunTEST.fbx"), ResourceManager::LoadDefaultMaterial()));
}

void TestScene::Update(float delta)
{
	LineRenderer& lines = renderSystem.lines;

	lines.SetColour({ 0.0f, 1.0f, 0.0f });
	lines.DrawLineSegment({ 0.0f, 0.0f, 0.0f }, { 10.0f, 0.0f, 10.0f });
	lines.DrawLineSegment({ 10.0f, 10.0f, 10.0f }, { -10.0f, -10.0f, -10.0f });
	lines.DrawLineSegment({ 10.0f, 10.0f, -10.0f }, { -10.0f, -10.0f, 10.0f });
	lines.DrawLineSegment({ 10.0f, -10.0f, 10.0f }, { -10.0f, 10.0f, -10.0f });
	lines.DrawLineSegment({ 10.0f, -10.0f, -10.0f }, { -10.0f, 10.0f, 10.0f });
	lines.DrawLineSegment({ -10.0f, 10.0f, 10.0f }, { 10.0f, -10.0f, -10.0f });
	lines.DrawLineSegment({ -10.0f, 10.0f, -10.0f }, { 10.0f, -10.0f, 10.0f });
	lines.DrawLineSegment({ -10.0f, -10.0f, 10.0f }, { 10.0f, 10.0f, -10.0f });


	lines.AddPointToLine({ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f });
	lines.AddPointToLine({ 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 0.0f });
	lines.AddPointToLine({ 2.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
	lines.AddPointToLine({ 3.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 1.0f });
	lines.AddPointToLine({ 4.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f });
	lines.AddPointToLine({ 5.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 1.0f });
	lines.AddPointToLine({ 6.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f });
	lines.FinishLineStrip();
	//if (animateBlendDirectionRight) {
	//	xbotBlendedAnimator.lerpAmount += delta;
	//	if (xbotBlendedAnimator.lerpAmount > 1.f) { xbotBlendedAnimator.lerpAmount = 1.f; }
	//}
	//else
	//{
		//xbotBlendedAnimator.lerpAmount -= delta;
		//if (xbotBlendedAnimator.lerpAmount < 0) { xbotBlendedAnimator.lerpAmount = 0; }
	//}



	messengerInterface.Update();

	pointLights[0].position.x = 1.5f * sinf((float)glfwGetTime() * 2.f);
	lightCube->transform()->setPosition(pointLights[0].position);
	spotlight.position = camera->transform.getPosition();
	spotlight.direction = camera->transform.forward();
}

void TestScene::Draw()
{
	//auto& xBotTransforms = xbotAnimator.getFinalBoneMatrices();
	//auto& xBotOtherTransforms = xbotOtherAnimator.getFinalBoneMatrices();
	
	//auto& xbotInterpolatedAnimations = xbotBlendedAnimator.getFinalBoneMatrices();

	renderSystem.Update(
		renderers,
		transforms,
		renderers,
		animators,
		camera
	);
}

void TestScene::OnMouseDown()
{

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


	if (!ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::End();
	}
	else {
		ImGui::DragFloat("Ortho Camera Zoom", &camera->orthoScale, 0.1f, 0, 100.0f);
		ImGui::SliderFloat("Animation trans", &xbotBlendedAnimator.lerpAmount, 0.f, 1.0f);
		if (ImGui::DragInt("Skybox Index", &skyboxIndex, 0.01f, 0, (unsigned int)(skyboxes.size() - 1))) {
			skybox = skyboxes[skyboxIndex];
			renderSystem.skyboxTexture = skybox->texture;
			renderSystem.IBLBufferSetup(skybox->texture);
		}

		ImGui::End();
	}

}


void TestScene::OnWindowResize()
{
	//glDeleteTextures(1, &textureColorbuffer);
}

TestScene::~TestScene()
{
	// TODO: Not deleting the textures properly
	// TODO: Move to Render System
}
