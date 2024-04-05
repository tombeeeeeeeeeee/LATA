#pragma once
#include "Part.h"

#include "Maths.h"

class Model;
class Shader;

class ModelRenderer : public Part
{
public:
	Model* model;

	ModelRenderer(Model* _model);

	virtual void Draw() override;
	virtual void GUI() override;
};

