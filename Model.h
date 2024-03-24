#pragma once
#include "Mesh.h"
#include "Shader.h"

class Model
{
public:
	Model();
	Model(std::string path);

	void LoadModel(std::string path);
	void AddMesh(Mesh* mesh);
	void Draw(Shader* shader);

	std::vector<Mesh> meshes;
private:
};

