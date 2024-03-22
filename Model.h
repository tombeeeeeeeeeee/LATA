#pragma once
#include "Mesh.h"
#include "Shader.h"

class Model
{
public:
	Model();
	Model(std::string path);

	void LoadModel(std::string path);
	void Draw(Shader* shader);

private:
	std::vector<Mesh> meshes;
	
	std::vector<Texture*> textures;
};

