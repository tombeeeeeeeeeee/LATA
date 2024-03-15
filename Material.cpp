#include "Material.h"

#include "glad.h"
#include "GLFW/glfw3.h"

Material::Material() :
	// Should these be specific values for unfound texture instead
	diffuseMap(-1),
	specularMap(-1)
{
}

Material::Material(unsigned int diffuse, unsigned int specular) :
	diffuseMap(diffuse),
	specularMap(specular)
{
}

void Material::Use()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);

	// Bind specular map
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specularMap);
}
