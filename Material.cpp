#include "Material.h"

#include "glad.h"
#include "GLFW/glfw3.h"

Material::Material() :
	// Should these be specific values for unfound texture instead
	diffuse(nullptr),
	specular(nullptr),
	emission(nullptr),
	shininess(64.0f)
{
}

Material::Material(Texture* _diffuse, Texture* _specular, Texture* _emission, float _shininess) :
	diffuse(_diffuse),
	specular(_specular),
	emission(_emission),
	shininess(_shininess)
{
}



void Material::Use()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuse->ID);

	// Bind specular map
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specular->ID);
}
