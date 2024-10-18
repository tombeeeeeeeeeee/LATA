#pragma once

#include "Texture.h"

#include "Maths.h"

#include <vector>
#include <unordered_map>

class Shader;
namespace toml {
	inline namespace v3 {
		class table;
	}
}

class Material
{
private:
	// TODO: store a bool to see if this has been run yet
	void GetShaderUniforms();
	Shader* shader;
	unsigned long long shaderGUID;
	std::unordered_map<std::string, unsigned long long> textureGUIDs;
public:
	std::unordered_map<std::string, Texture*> texturePointers;
	Material(std::string _name, Shader* _shader);
	void setShader(Shader* _shader);
	Shader* getShader();
	unsigned long long GUID;
	std::string name;
	glm::vec3 colour = { 1.0f ,1.0f ,1.0f };
	
	// TODO: Think about better ways to have this rather than having a map for each type
	std::unordered_map<std::string, float> floats;
	void AddTextures(std::vector<Texture*> _textures);
	void Use(Shader* shader = nullptr);

	Texture* getFirstTextureOfType(Texture::Type type) const;

	// Will re get all of the texture pointers using the texture GUIDs
	void Refresh();

	void GUI();

	void OpenModal();
	bool modalJustOpened = false;

	void ModalGUI();

	operator std::string() const;
	std::string getDisplayName() const;

	toml::table Serialise() const;
	void SaveAsAsset() const;
};

