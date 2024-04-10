#pragma once
#include "Part.h"

#include "Maths.h"

class Model;
class Shader;
class Material;

class ModelRenderer : public Part
{
public:
	Model* model;
	Material* material;
	unsigned long long materialGUID;

	ModelRenderer(Model* _model, unsigned long long _materialGUID);
	ModelRenderer(Model* _model, Material* _material);

	virtual void Draw() override;
	virtual void GUI() override;
};

