#pragma once

#include "Model.h"
#include "Shader.h"

// TODO: There are other ways to do a skybox, see: https://webglfundamentals.org/webgl/lessons/webgl-skybox.html for an example
class Skybox
{
public:
	Shader* shader;
	unsigned long long shaderGUID;
	Mesh box;
	GLuint texture;
	
	Skybox(Shader* _shader, GLuint _texture);
	
	void Update(glm::mat4 translationLessVP);
	void Draw();
};

