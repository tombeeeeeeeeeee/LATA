#pragma once

#include "Model.h"
#include "Shader.h"

class Camera;

// TODO: There are other ways to do a skybox, see: https://webglfundamentals.org/webgl/lessons/webgl-skybox.html for an example
class Skybox
{
public:
	Shader* shader;
	unsigned long long shaderGUID;
	GLuint texture;
	
	Skybox(Shader* _shader, GLuint _texture);
	
	void Update(Camera* camera, float ratio);
	void Update(glm::mat4 translationLessVP);
	void Draw();

	void Refresh();
};

