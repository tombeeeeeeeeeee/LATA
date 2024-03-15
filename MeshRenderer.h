#pragma once
#include "Mesh.h"
#include "Material.h"

class MeshRenderer
{
public:
	Material& material;
	Mesh& mesh;

	MeshRenderer(Material& _material, Mesh& _mesh);

	void Draw();
};

