#pragma once
#include "ModelRenderer.h"

#include "Transform.h"

#include <vector>

class MultiModelRenderer : public ModelRenderer
{
public:
	std::vector<Transform> transforms;

	void Draw() override;
	
	MultiModelRenderer(Model* _model, Material* _material);
	MultiModelRenderer(Model* _model, Material* _material, std::vector<Transform> _tranforms);

	void GUI() override;
};

