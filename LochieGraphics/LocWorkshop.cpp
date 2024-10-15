#include "LocWorkshop.h"

#include "Camera.h"
#include "SceneObject.h"
#include "ResourceManager.h"
#include "Paths.h"
#include "ComputeShader.h"
#include "Animation.h"

#include "EditorGUI.h"
#include "ExtraEditorGUI.h"

#include <iostream>

LocWorkshop::LocWorkshop()
{
}

void LocWorkshop::Start()
{
	lights.insert(lights.end(), { &directionalLight });

	camera->editorSpeed.move = 250.0f;
	camera->farPlane = 500.0f;
	camera->nearPlane = 1.0f;
	camera->transform.setPosition({ 0.0f, 0.0f, 150.0f });
	camera->transform.setEulerRotation({ 0.0f, 90.0f, 0.0f });


	int max_compute_work_group_count[3] = {};
	int max_compute_work_group_size[3] = {};
	int max_compute_work_group_invocations = {};

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

	std::cout << "Number of invocations in a single local work group that may be dispatched to a compute shader " << max_compute_work_group_invocations << "\n\n";

	texture = ResourceManager::LoadTexture(1024u, 1024u);
	texture->type = Texture::Type::albedo;
	texture->Bind(0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 1024u, 1024u, 0, GL_RGBA, GL_FLOAT, NULL);


	shader = ResourceManager::LoadShader("particle", Shader::Flags::VPmatrix);
	shaders.push_back(shader);
	Material* material = ResourceManager::LoadMaterial("Testing", shader);
	material->AddTextures({ texture });

	sceneObject = new SceneObject(this, "Testing!");
	model = ResourceManager::LoadModelAsset(Paths::modelSaveLocation + "SM_FloorTile" + Paths::modelExtension);
	//sceneObject->setRenderer(new ModelRenderer(model, material));

	texture->Bind(0);
	glBindImageTexture(0, texture->GLID, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);

	quad = new Mesh();
	quad->InitialiseQuad(1);

	particle.shader = shader;
	particle.Initialise();

	camera->nearPlane = 10.0f;
	camera->farPlane = 50000.0f;

	Texture* particleTexture = ResourceManager::LoadTexture("images/brodie.jpg", Texture::Type::albedo);

	particle.texture = particleTexture;
	particle.moveCompute =    new ComputeShader(Paths::shadersSaveLocation + "particleMove"    + Paths::computeExtension);
	particle.resetCompute =   new ComputeShader(Paths::shadersSaveLocation + "particleReset"   + Paths::computeExtension);
	particle.explodeCompute = new ComputeShader(Paths::shadersSaveLocation + "particleExplode" + Paths::computeExtension);
	particle.stopCompute =    new ComputeShader(Paths::shadersSaveLocation + "particleStop"    + Paths::computeExtension);
	particle.spreadCompute =  new ComputeShader(Paths::shadersSaveLocation + "particleSpread"  + Paths::computeExtension);
}

void LocWorkshop::Update(float delta)
{
	particle.Update(delta);
}

void LocWorkshop::Draw()
{
	


	renderSystem.Update(
		renderers,
		transforms,
		renderers,
		animators,
		camera,
		{ &particle }
	);

	//texture->Bind(0);
	//glBindImageTexture(0, texture->GLID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);


	//computeShader->Use();
	//glDispatchCompute((unsigned int)texture->width, (unsigned int)texture->height, 1);

	//// make sure writing to image has finished before read
	//glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);



	////glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//shader->Use();
	//shader->setInt("material.albedo", 0);
	//texture->Bind(0);

	//glDisable(GL_DEPTH_TEST);

	//glDisable(GL_CULL_FACE);

	//shader->setMat4("model", glm::identity<glm::mat4>());
	//quad->Draw();

	////model->Draw();
}

void LocWorkshop::GUI()
{
	if (!ImGui::Begin("Particle stuff", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::End();
		return;
	}
	
	ImGui::DragFloat("Explode Strength", &particle.explodeStrength);

	if (ImGui::Button("Reset##Particles")) {
		particle.Reset();
	}

	if (ImGui::Button("Explode##Particles")) {
		particle.Explode();
	}

	if (ImGui::Button("Initialise##Particles")) {
		particle.Initialise();
	}

	if (ImGui::Button("Spread##Particles")) {
		particle.Spread();
	}

	if (ImGui::Button("Stop##Particle")) {
		particle.Stop();
	}

	ImGui::Checkbox("Facing Camera##Particle", &renderSystem.particleFacingCamera);

	//ImGui::DragScalar("Count##Particle", ImGuiDataType_U32, reinterpret_cast<void*>(&particle.count));

	ExtraEditorGUI::Mat4Input("Model Matrix##Particle", &particle.model);


	ImGui::End();
}
