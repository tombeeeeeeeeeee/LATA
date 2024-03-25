#pragma once

#include "Maths.h"

class Model;
class Shader;

class ModelRenderer
{
public:
	Model* model;
	Shader* shader;

	ModelRenderer(Model* _model, Shader* _shader);

	virtual void Draw(glm::mat4 matrix);
};

