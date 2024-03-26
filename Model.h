#pragma once
#include "Mesh.h"
#include "Shader.h"

class Model
{
public:
	Model();
	Model(std::string path, bool flipTexturesOnLoad = true);

	void LoadModel(std::string path, bool flipTexturesOnLoad = true);
	void AddMesh(Mesh* mesh);
	void Draw(Shader* shader);
	void SetMaterial(Material* material);
private:
	//TODO: Model currently comepletly owns the meshes, this should not be the case
	std::vector<Mesh> meshes;
};

