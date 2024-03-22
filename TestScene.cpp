#include "TestScene.h"

//TODO: this should be later removed as translations shouldnt be happening here
#include "gtc/matrix_transform.hpp"


void TestScene::Start()
{
	// Create shaders
	lightingShader = ResourceManager::GetShader("shader.vert", "shader.frag");
	lightCubeShader = ResourceManager::GetShader("lightCube.vert", "lightCube.frag");

	boxMesh.InitialiseCube();
	lightCubeMesh.InitialiseCube();
	quadMesh.InitialiseQuad();
	grassMesh.InitialiseDoubleSidedQuad();

	Texture* diffuseMap = ResourceManager::GetTexture("images/container2.png", Texture::Type::diffuse);
	boxMesh.textures.push_back(diffuseMap);
	Texture* specularMap = ResourceManager::GetTexture("images/container2_specular.png", Texture::Type::specular);
	boxMesh.textures.push_back(specularMap);
	Texture* grass = ResourceManager::GetTexture("images/grass.png", Texture::Type::diffuse, GL_CLAMP_TO_EDGE);
	grassMesh.textures.push_back(grass);
	testLocModel = Model("models/backpack/backpack.obj");

	backpack = SceneObject(&testLocModel, lightingShader, { 0.f, 1.f, 1.f });

	// shader configuration
	lightingShader->Use();
	lightingShader->setFloat("material.shininess", 64.0f);

	// TODO: Clean up this light setup, could be functions somewhere else?
	// directional light
	lightingShader->setVec3("dirLight.direction", directionalLight.direction);
	lightingShader->setVec3("dirLight.ambient", directionalLight.ambient);
	lightingShader->setVec3("dirLight.diffuse", directionalLight.diffuse);
	lightingShader->setVec3("dirLight.specular", directionalLight.specular);

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

	lightingShader->setVec3("spotlight.position", spotlight.position);
	lightingShader->setVec3("spotlight.ambient", spotlight.ambient);
	lightingShader->setVec3("spotlight.diffuse", spotlight.diffuse);
	lightingShader->setVec3("spotlight.specular", spotlight.specular);
	lightingShader->setFloat("spotlight.constant", spotlight.constant);
	lightingShader->setFloat("spotlight.linear", spotlight.linear);
	lightingShader->setFloat("spotlight.quadratic", spotlight.quadratic);
	lightingShader->setFloat("spotlight.cutOff", spotlight.cutOff);
	lightingShader->setFloat("spotlight.outerCutOff", spotlight.outerCutOff);
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

	glm::mat4 model = glm::mat4(1.0f);


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
