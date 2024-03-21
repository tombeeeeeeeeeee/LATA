#pragma once

#include "Material.h"

#include <set>

/*


Material is vector of textures

Shouldn't have two of the same of the materials but can

When a model is loaded, each mesh has a pointer to each texture that that mesh needs
Instead, each mesh could could store a material index (not pointer cause pointer would prob change as a set is being used)





*/



class MaterialManager
{
public:
	std::set<Material*> loadedTextures;

	Material* getMaterial(unsigned int i);

	//Material* getMaterial

private:

	
};

