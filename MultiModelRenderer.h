#pragma once
#include "ModelRenderer.h"

#include "Transform.h"

#include <vector>

class MultiModelRenderer : public ModelRenderer
{
public:
	std::vector<Transform> transforms;

	void Draw() override;
	
	MultiModelRenderer(Model* _model, Shader* _shader);
	MultiModelRenderer(Model* _model, Shader* _shader, std::vector<Transform> _tranforms);

	void GUI() override;
};

