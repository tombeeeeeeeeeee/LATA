#include "Skybox.h"

#include "Shader.h"
#include "Camera.h"

#include "ResourceManager.h"

Skybox::Skybox(Shader* _shader, GLuint _texture) :
	shader(_shader),
	texture(_texture)
{
	shaderGUID = shader->GUID;
}

void Skybox::Update(Camera* camera, float ratio)
{
	glm::mat4 view = glm::mat4(glm::mat3(camera->GetViewMatrix()));
	// TODO: This math shouldn't be here, maybe move to camera class or get the projection from scenemanager
	glm::mat4 projection = glm::perspective(glm::radians(camera->fov), ratio, camera->nearPlane, camera->farPlane);
	glm::mat4 skyBoxVP = projection * view;
	Update(skyBoxVP);

}

void Skybox::Update(glm::mat4 translationLessVP)
{
	shader->Use();
	shader->setMat4("vp", translationLessVP);
}

void Skybox::Draw()
{
	glDepthFunc(GL_LEQUAL); // Change depth function
	Texture::UseCubeMap(texture, shader);
	
	glDepthFunc(GL_LESS); // Change depth function back //TODO: Can the depth function just stay lequal
}

void Skybox::Refresh()
{
	shader = ResourceManager::GetShader(shaderGUID);
}
