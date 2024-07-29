#include "Skybox.h"

Skybox::Skybox(Shader* _shader, GLuint _texture) :
	shader(_shader),
	texture(_texture)
{
	shaderGUID = shader->GUID;
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
