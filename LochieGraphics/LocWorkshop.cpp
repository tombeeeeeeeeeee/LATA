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

	//particles.resize(2);
	//for (auto& i : particles)
	//{
	//	i.shader = shader;
	//	i.Initialise();
	//}

	camera->nearPlane = 10.0f;
	camera->farPlane = 50000.0f;

	particleTexture = ResourceManager::LoadTexture("images/brodie.jpg", Texture::Type::albedo);

	//for (auto& i : particles)
	//{
	//	i.texture = particleTexture;
	//}
}

void LocWorkshop::Update(float delta)
{
	std::vector<std::vector<Particle*>::iterator> toDelete;
	for (auto i = particles.begin(); i != particles.end(); i++)
	{
		(*i)->Update(delta);
		if ((*i)->lifetime < 0) {
			toDelete.push_back(i);
		}
	}

	
	// TODO: There is prob a better way to do this
	// Deleting backwards to avoid invaliding iterators
	for (auto i = toDelete.rbegin(); i != toDelete.rend(); i++)
	{
		delete **i;
		particles.erase(*i);
	}
}

void LocWorkshop::Draw()
{


	renderSystem.Update(
		renderers,
		transforms,
		renderers,
		animators,
		camera,
		particles
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

	for (size_t i = 0; i < particles.size(); i++)
	{
		if (ImGui::CollapsingHeader(("P" + std::to_string(i)).c_str())) {
			ImGui::Indent();
			particles.at(i)->GUI();
			ImGui::Unindent();
		}
	}

	ImGui::DragInt("Count", &nextParticleCount, 1.0f, 1, INT_MAX);

	ImGui::DragFloat("Lifetime", &nextParticleLifetime);

	ResourceManager::TextureSelector("Texture", &particleTexture, false);

	if (ImGui::Button("Add Another Particles")) {
		particles.emplace_back(new Particle( nextParticleCount, nextParticleLifetime, shader, particleTexture ));
		particles.back()->Initialise();
	}



	ImGui::End();
}
