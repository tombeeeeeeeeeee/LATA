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
	// TODO: Should hold multiple materials, one for each mesh from the model, 
	// I think that there are material slots, so it should be for how ever many materials the model needs
	Material* material;
	unsigned long long materialGUID;

	ModelRenderer(Model* _model, unsigned long long _materialGUID);
	ModelRenderer(Model* _model, Material* _material);

	virtual void Draw(Shader* override) override;
	virtual void GUI() override;
};

