#pragma once
#include "Part.h"

#include "Maths.h"

class Model;
class Shader;

class ModelRenderer : public Part
{
public:
	Model* model;
	Shader* shader;

	ModelRenderer(Model* _model, Shader* _shader);

	virtual void Draw() override;
	virtual void GUI(Part* part) override;
};

