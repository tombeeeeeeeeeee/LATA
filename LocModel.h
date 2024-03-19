#pragma once
#include "Mesh.h"
#include "Shader.h"

class LocModel
{
public:
	LocModel();
	LocModel(std::string path);

	void LoadModel(std::string path);
	void Draw(Shader& shader);

private:
	std::vector<Mesh> meshes;
};

