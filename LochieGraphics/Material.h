#pragma once

#include "Texture.h"

#include <vector>
#include <unordered_map>

class Shader;

class Material
{
private:
	// TODO: store a bool to see if this has been run yet
	void GetShaderUniforms();
	Shader* shader;
	std::unordered_map<std::string, Texture*> textures;
public:
	Material(std::string _name, Shader* _shader);
	Material(std::string _name);
	void setShader(Shader* _shader);
	Shader* getShader();
	std::string name; // TODO: Create some other sort of identifier, like an ID or something

	
	// TODO: Think about better ways to have this rather than having a map for each type
	std::unordered_map<std::string, float> floats;
	void AddTextures(std::vector<Texture*> _textures);
	void Use();

	void GUI();
};

