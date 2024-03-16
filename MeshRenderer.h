#pragma once
#include "Mesh.h"
#include "Material.h"

class MeshRenderer
{
public:
	Material* material;
	Mesh* mesh;
	Shader* shader;

	MeshRenderer(Material* _material, Mesh* _mesh, Shader* _shader);
	MeshRenderer();

	void Draw();
};

