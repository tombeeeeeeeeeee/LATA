#include "Material.h"

#include "glad.h"
#include "GLFW/glfw3.h"

Material::Material() :
	// Should these be specific values for unfound texture instead
	diffuse(nullptr),
	specular(nullptr),
	shader(nullptr)
{
}

Material::Material(Texture* _diffuse, Texture* _specular, Shader* _shader) :
	diffuse(_diffuse),
	specular(_specular),
	shader(_shader)
{
}

void Material::Use()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuse->ID);

	// Bind specular map
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specular->ID);

	shader->Use();
}
