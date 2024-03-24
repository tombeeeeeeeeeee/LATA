#include "TestScene.h"


void TestScene::Start()
{
	// Create shaders
	lightingShader = ResourceManager::GetShader("shader.vert", "shader.frag");
	lightCubeShader = ResourceManager::GetShader("lightCube.vert", "lightCube.frag");

	boxMesh.InitialiseCube();
	lightCubeMesh.InitialiseCube();
	grassMesh.InitialiseDoubleSidedQuad();

	Texture* diffuseMap = ResourceManager::GetTexture("images/container2.png", Texture::Type::diffuse);
	Texture* specularMap = ResourceManager::GetTexture("images/container2_specular.png", Texture::Type::specular);
	boxMesh.material = ResourceManager::GetMaterial(std::vector<Texture*>{ diffuseMap, specularMap });
	
	Texture* grass = ResourceManager::GetTexture("images/grass.png", Texture::Type::diffuse, GL_CLAMP_TO_EDGE);
	grassMesh.material = ResourceManager::GetMaterial(std::vector<Texture*>{ grass });
	backpackModel = Model("models/backpack/backpack.obj");

	backpack = SceneObject(&backpackModel, lightingShader, { 0.f, 1.f, 1.f });
	Mesh newMesh;
	newMesh.material = ResourceManager::GetMaterial(std::vector<Texture*>{ diffuseMap, specularMap });
	newMesh.InitialiseCube();
	boxModel.AddMesh(&newMesh);
	//boxModel.meshes.push_back(newMesh);
	//boxModel.meshes.push_back(std::move(newMesh));
	//boxModel.meshes[0].material = ResourceManager::GetMaterial(std::vector<Texture*>{ diffuseMap, specularMap });
	//boxModel.AddMesh();

	// shader configuration
	lightingShader->Use();
	lightingShader->setFloat("material.shininess", 64.0f);

	// directional light
	directionalLight.ApplyToShader(lightingShader);

	// TODO: Clean up this light setup, could be functions somewhere else?
	// Point lights
	for (int i = 0; i < sizeof(pointLights) / sizeof(pointLights[0]); i++)
	{
		std::string index = std::to_string(i);
		lightingShader->setVec3("pointLights[" + index + "].position", pointLights[i].position);
		lightingShader->setVec3("pointLights[" + index + "].ambient", pointLights[i].ambient);
		lightingShader->setVec3("pointLights[" + index + "].diffuse", pointLights[i].diffuse);
		lightingShader->setVec3("pointLights[" + index + "].specular", pointLights[i].specular);
		lightingShader->setFloat("pointLights[" + index + "].constant", pointLights[i].constant);
		lightingShader->setFloat("pointLights[" + index + "].linear", pointLights[i].linear);
		lightingShader->setFloat("pointLights[" + index + "].quadratic", pointLights[i].quadratic);
	}

	spotlight.ApplyToShader(lightingShader);
}

void TestScene::Update(float delta)
{
	// light properties
	lightingShader->Use();
	lightingShader->setVec3("spotlight.position", camera->position);
	lightingShader->setVec3("spotlight.direction", camera->front);

	//
	lightingShader->setVec3("viewPos", camera->position);
	lightingShader->setMat4("vp", viewProjection);

	lightCubeShader->Use();
	lightCubeShader->setMat4("vp", viewProjection);

	lightingShader->Use();

	glm::mat4 model = glm::mat4(5.0f);
	lightingShader->setMat4("model", model);
	boxModel.Draw(lightingShader);

	backpack.Draw();

	lightingShader->Use();
	// render containers
	for (unsigned int i = 0; i < 10; i++)
	{
		// calculate the model matrix for each object and pass it to shader before drawing
		model = glm::mat4(1.0f);
		model = glm::translate(model, cubePositions[i]);
		float angle = 20.0f * i;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		lightingShader->setMat4("model", model);

		boxMesh.Draw(lightingShader);
	}

	// render grass
	for (unsigned int i = 0; i < 10; i++)
	{
		// calculate the model matrix for each object and pass it to shader before drawing
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, grassPositions[i]);
		float angle = 20.0f * i;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
		lightingShader->setMat4("model", model);

		grassMesh.Draw(lightingShader);
	}

	// also draw the lights themselves
	lightCubeShader->Use();
	lightCubeShader->setMat4("vp", viewProjection);

	// we now draw as many light bulbs as we have point lights.
	for (unsigned int i = 0; i < 4; i++)
	{
		model = glm::mat4(1.0f);
		model = glm::translate(model, pointLights[i].position);
		model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
		lightCubeShader->setMat4("model", model);

		lightCubeMesh.Draw(lightCubeShader);
	}
}

TestScene::~TestScene()
{
	
}
