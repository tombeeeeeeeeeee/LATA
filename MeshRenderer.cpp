#include "MeshRenderer.h"


MeshRenderer::MeshRenderer(Material& _material, Mesh& _mesh) :
	material(_material),
	mesh(_mesh)
{
}

void MeshRenderer::Draw()
{
	material.Use();
	mesh.Draw();
}
