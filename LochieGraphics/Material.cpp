#include "Material.h"

#include "Shader.h"
#include "ResourceManager.h"

#include "Utilities.h"

#include "Graphics.h"
#include "imgui.h"
#include "imgui_stdlib.h"

#include <iostream>

void Material::GetShaderUniforms()
{
	if (!shader) {
		std::cout << "Tried to get uniforms of no particular shader! On material:" << name << "\n";
		return;
	}
	// TODO: Clear current stuff
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
	GetShaderUniforms();
}

void Material::AddTextures(std::vector<Texture*> _textures)
{
	for (auto i = _textures.begin(); i != _textures.end(); i++)
	{
		// TODO: Only one of each texture is supported atm, fix?
		// TODO: what happens when not found
		//auto texture = textures.find("material." + Texture::TypeNames.find((*i)->type)->second + "1");
		std::string textureName = "material." + Texture::TypeNames.find((*i)->type)->second + "1";
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

void Material::Use()
{
	// Bind textures

	shader->Use();
	// Unbinds any extra assigned textures, this is so if a mesh only has one diffuse, the previously set specular from another mesh isn't used.
	//for (unsigned int i = textures.size(); i < textures.size() + 5; i++)
	for (unsigned int i = 0; i < 10; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i );
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	int count = 1;
	for (auto i = texturePointers.begin(); i != texturePointers.end(); i++)
	{
		if (i->second == nullptr) { 
			continue; 
		}
		glActiveTexture(GL_TEXTURE0 + count);
		shader->setSampler(i->first, count);
		glBindTexture(GL_TEXTURE_2D, i->second->GLID);
		count++;
	}
	for (auto i = floats.begin(); i != floats.end(); i++)
	{
		shader->setFloat(i->first, i->second);
	}
}

void Material::Refresh()
{
	for (auto i = textureGUIDs.begin(); i != textureGUIDs.end(); i++)
	{
		if (i->second) {
			texturePointers.find(i->first)->second = ResourceManager::GetTexture(i->second);
		}
	}
}

void Material::GUI()
{
	std::string tag = PointerToString(this);
	ImGui::Text(name.c_str());
	ImGui::Text(("Shader gl ID:" + std::to_string(shader->GLID)).c_str());
	for (auto i = textureGUIDs.begin(); i != textureGUIDs.end(); i++)
	{
		
		//ImGui::Text(i->first.c_str());
		std::string number = std::to_string(i->second);
		// TODO: Make it so only numbers can be typed in this
		if (ImGui::InputText((i->first + "##" + PointerToString(&i->second)).c_str(), &number)) {
			i->second = std::stoull(number);
			Refresh();
		}




		//if (i->second) {
		//	ImGui::SameLine();

		//	ImGui::Text(std::to_string(i->second->GLID).c_str()); // TODO: make this customisable
		//}
	}
	for (auto i = floats.begin(); i != floats.end(); i++)
	{
		ImGui::Text(i->first.c_str());
		ImGui::DragFloat((i->first + "##" + tag).c_str(), &i->second);
	}
	
}
