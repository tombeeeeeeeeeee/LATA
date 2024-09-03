#include "Material.h"

#include "ResourceManager.h"

#include "Utilities.h"

#include "EditorGUI.h"

#include <iostream>

using Utilities::PointerToString;

void Material::GetShaderUniforms()
{
	if (!shader) {
		std::cout << "Tried to get uniforms of no particular shader! On material:" << name << "\n";
		return;
	}
	// TODO: Clear current stuff
	texturePointers.clear();
	textureGUIDs.clear();
	floats.clear();

	GLint uniformCount;

	glGetProgramiv(shader->GLID, GL_ACTIVE_UNIFORMS, &uniformCount);
	for (GLint i = 0; i < uniformCount; i++)
	{
		GLint size; // size of the variable
		GLenum type; // type of the variable (float, vec3 or mat4, etc)
		const GLsizei bufSize = 32; // maximum name length
		GLchar name[bufSize]; // variable name in GLSL
		GLsizei length; // name length

		glGetActiveUniform(shader->GLID, i, bufSize, &length, &size, &type, name);
		// If the uniform doesn't start with 'material.', ignore it
		if (strncmp(name, "material.", 9) != 0) { continue; }

		// TODO: does the whole name need to be inserted, is it worth cutting out the 'material.'
		switch (type)
		{
		case GL_SAMPLER_2D:
			// TODO: This is prob where a default texture can be set
			texturePointers.emplace(name, (Texture*)nullptr);
			textureGUIDs.emplace(name, 0);
			break;
		case GL_FLOAT:
			floats.emplace(name, 32.f);
			break;
		default:
			std::cout << "Error: Shader uniform type not yet supported for material! Type: " << type << "\n";
			break;
		}
	}
}

Shader* Material::getShader()
{
	return shader;
}

void Material::setShader(Shader* _shader)
{
	shader = _shader;
	// TODO: should this clear them first
	GetShaderUniforms();
}

Material::Material(std::string _name, Shader* _shader) :
	shader(_shader),
	name(_name)
{
	shaderGUID = _shader->GUID;
	GetShaderUniforms();
}

void Material::AddTextures(std::vector<Texture*> _textures)
{
	for (auto i = _textures.begin(); i != _textures.end(); i++)
	{
		// TODO: Only one of each texture is supported atm, fix?
		// TODO: what happens when not found
		//auto texture = textures.find("material." + Texture::TypeNames.find((*i)->type)->second + "1");
		std::string textureName = "material." + Texture::TypeNames.find((*i)->type)->second;
		auto texture = texturePointers.find(textureName);
		if (texture != texturePointers.end()) {
			texture->second = (*i);
			textureGUIDs.find(textureName)->second = texture->second->GUID;
		}
		else {
			std::cout << "Error, unable to find texture spot for material: " << textureName + "\n";
		}
	}
}

void Material::Use(Shader* _shader)
{
	// Bind textures
	Shader* currshader;
	if (_shader == nullptr)
		currshader = shader;
	else
		currshader = _shader;
	currshader->Use();
	// Unbinds any extra assigned textures, this is so if a mesh only has one diffuse, the previously set specular from another mesh isn't used.
	//for (unsigned int i = textures.size(); i < textures.size() + 5; i++)
	// TODO: This might not be needed anymore
	//for (unsigned int i = 0; i < 10; i++)
	//{
	//	Texture::ClearBind(i);
	//}

	int count = 1;
	for (auto i = texturePointers.begin(); i != texturePointers.end(); i++)
	{
		currshader->setSampler(i->first, count);
		if (i->second) {
			i->second->Bind(count);
		}
		else {
			Texture::ClearBind(count);
		}
		count++;
	}
	for (auto i = floats.begin(); i != floats.end(); i++)
	{
		currshader->setFloat(i->first, i->second);
	}
}

Texture* Material::getFirstTextureOfType(Texture::Type type) const
{
	for (auto i = texturePointers.begin(); i != texturePointers.end(); i++)
	{
		if (!i->second) { continue; }
		if (i->second->type == type) {
			return i->second;
		}
	}
	return nullptr;
}

void Material::Refresh()
{
	shader = ResourceManager::GetShader(shaderGUID);
	for (auto i = textureGUIDs.begin(); i != textureGUIDs.end(); i++)
	{
		if (i->second) {
			texturePointers.find(i->first)->second = ResourceManager::GetTexture(i->second);
		}
		else {
			texturePointers.find(i->first)->second = nullptr;
		}
	}
}


void Material::GUI()
{
	std::string tag = PointerToString(this);
	ImGui::Text(("GUID: " + std::to_string(GUID)).c_str());
	ImGui::InputText(("Name##" + tag).c_str(), &name);

	if (ResourceManager::ShaderSelector("Shader", &shader)) {
		shaderGUID = shader->GUID;
		setShader(shader);
	}
	
	ImGui::ColorEdit3(("Colour Picker##" + tag).c_str(), &colour[0]);
	//ImGui::

	for (auto i = texturePointers.begin(); i != texturePointers.end(); i++)
	{
		if (ResourceManager::TextureSelector((i->first + "##" + PointerToString(&i->second)).c_str(), &i->second)) {
			if (i->second) {
				textureGUIDs[i->first] = i->second->GUID;
			}
			else {
				textureGUIDs[i->first] = 0;
			}
			Refresh();
		}
	}
	for (auto i = floats.begin(); i != floats.end(); i++)
	{
		ImGui::DragFloat((i->first + "##" + tag).c_str(), &i->second);
	}

	if (ImGui::Button(("Open modal##" + tag).c_str())) {
		OpenModal();
	}
}

void Material::OpenModal()
{
	modalJustOpened = true;
}

void Material::ModalGUI()
{
	if (modalJustOpened) {
		ImGui::OpenPopup(("Material Modal##" + PointerToString(this)).c_str());
	}
	if (!ImGui::BeginPopupModal(("Material Modal##" + PointerToString(this)).c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		return;
	}
	std::string tag = PointerToString(this);
	ImGui::Text(("GUID: " + std::to_string(GUID)).c_str());
	if (modalJustOpened) {
		ImGui::SetKeyboardFocusHere();
		modalJustOpened = false;
	}
	if (ImGui::InputText(("Name##" + tag).c_str(), &name, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue)) {
		ImGui::CloseCurrentPopup();
	}

	if (ImGui::Button(("Done##" + tag).c_str())) {
		ImGui::CloseCurrentPopup();
	}

	ImGui::EndPopup();
}

std::string Material::getDisplayName() const
{
	return name + " " + std::to_string(GUID);
}

toml::table Material::Serialise() const
{
	toml::array savedTextures;
	for (auto& i : textureGUIDs)
	{
		savedTextures.push_back(toml::table{
			{i.first, Serialisation::SaveAsUnsignedLongLong(i.second)}
			});
	}
	toml::array savedFloats;
	for (auto& i : floats)
	{
		savedFloats.push_back(toml::table{
			{i.first, i.second}
			});
	}

	return toml::table{
		{ "name", name },
		{ "guid", Serialisation::SaveAsUnsignedLongLong(GUID)},
		{ "shader", Serialisation::SaveAsUnsignedLongLong(shaderGUID) },
		{ "colour", Serialisation::SaveAsVec3(colour)},
		{ "textures", savedTextures},
		{ "floats", savedFloats}
	};
}
