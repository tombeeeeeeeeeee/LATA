#pragma once
#include "Serialisation.h"

#include <vector>

class Model;
class Shader;
class Material;

class ModelRenderer
{
public:
	Model* model;
	unsigned long long modelGUID;
	// TODO: Should hold multiple materials, one for each mesh from the model, 
	// I think that there are material slots, so it should be for how ever many materials the model needs

	std::vector<Material*> materials = {};
	std::vector<unsigned long long> materialGUIDs = {};

	//ASK: Made to ease errors with unordered maps
	ModelRenderer();

	ModelRenderer(Model* _model, unsigned long long _materialGUID);
	ModelRenderer(Model* _model, Material* _material);

	void GUI();

	toml::table Serialise(unsigned long long GUID) const;
	ModelRenderer(toml::table table);

	void Refresh();
};

