#include "MeshRenderer.h"


MeshRenderer::MeshRenderer(Material* _material, Mesh* _mesh, Shader* _shader) :
	material(_material),
	mesh(_mesh),
	shader(_shader)
{
}

MeshRenderer::MeshRenderer() : 
	material(nullptr),
	mesh(nullptr),
	shader(nullptr)
{
}

void MeshRenderer::Draw()
{
	shader->Use();
	material->Use();
	mesh->Draw();
}
