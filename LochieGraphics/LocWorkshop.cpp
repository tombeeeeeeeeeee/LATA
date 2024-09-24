#include "LocWorkshop.h"

#include "Camera.h"
#include "SceneObject.h"
#include "ResourceManager.h"
#include "Paths.h"
#include "ComputeShader.h"
#include "Animation.h"

LocWorkshop::LocWorkshop()
{
}

void LocWorkshop::Start()
{
	lights.insert(lights.end(), { &directionalLight });

	camera->editorSpeed.move = 250.0f;
	camera->farPlane = 500.0f;
	camera->nearPlane = 1.0f;
	camera->transform.setPosition({ 0.0f, 400.0f, 0.0f });
	camera->transform.setEulerRotation({ 0.0f, 0.0f, -90.0f });

	sceneObject = new SceneObject(this, "Testing!");
	//sceneObject->setRenderer(new ModelRenderer(ResourceManager::LoadModelAsset(Paths::modelSaveLocation + "SM_FloorTile" + Paths::modelExtension), ResourceManager::defaultMaterial));

	
	int max_compute_work_group_count[3];
	int max_compute_work_group_size[3];
	int max_compute_work_group_invocations;

	for (int idx = 0; idx < 3; idx++) {
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, idx, &max_compute_work_group_count[idx]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, idx, &max_compute_work_group_size[idx]);
	}
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &max_compute_work_group_invocations);

	std::cout << "OpenGL Limitations: " << '\n';
	std::cout << "Maximum number of work groups in X dimension " << max_compute_work_group_count[0] << '\n';
	std::cout << "Maximum number of work groups in Y dimension " << max_compute_work_group_count[1] << '\n';
	std::cout << "Maximum number of work groups in Z dimension " << max_compute_work_group_count[2] << '\n';

	std::cout << "Maximum size of a work group in X dimension " << max_compute_work_group_size[0] << '\n';
	std::cout << "Maximum size of a work group in Y dimension " << max_compute_work_group_size[1] << '\n';
	std::cout << "Maximum size of a work group in Z dimension " << max_compute_work_group_size[2] << '\n';

	std::cout << "Number of invocations in a single local work group that may be dispatched to a compute shader " << max_compute_work_group_invocations << '\n\n';

	//ComputeShader computeShader = ComputeShader("shaders/LocsComputeShader.comp");


	Texture* texture = ResourceManager::LoadTexture(1024u, 1024u);

	//sceneObject->renderer()->materials.front()->texturePointers["material.albedo"] = texture;

	aniTest = new SceneObject(this, "Animation Test");
	Model* aniTestModel = ResourceManager::LoadModel("C:/Users/s220518/Downloads/test/SM_X Bot.fbx");
	aniTest->setRenderer(new ModelRenderer(aniTestModel, ResourceManager::defaultMaterial));

	animation = Animation("C:/Users/s220518/Downloads/test/SM_Chicken Dance.fbx", aniTestModel);

	animator = new Animator(&animation);
	aniTest->setAnimator(animator);
}

void LocWorkshop::Update(float delta)
{
}

void LocWorkshop::Draw()
{
	renderSystem.Update(
		renderers,
		transforms,
		renderers,
		animators,
		camera
	);
}

void LocWorkshop::GUI()
{
}
