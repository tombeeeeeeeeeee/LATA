#pragma once
#include "Mesh.h"
#include "Shader.h"



class LocModel
{
public:

	LocModel();
	LocModel(std::string path);

	void LoadModel(std::string path);

	std::vector<Mesh> meshes;

	void Draw(Shader& shader);
};

