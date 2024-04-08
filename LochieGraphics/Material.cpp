#include "Material.h"

#include "Shader.h"

#include "Utilities.h"

#include "Graphics.h"
#include "imgui.h"

#include <iostream>

void Material::GetShaderUniforms()
{
	if (!shader) {
		std::cout << "Tried to get uniforms of no shader! On material:" << name << "\n";
		return;
	}
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
			textures.emplace(name, (Texture*)nullptr);
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

Material::Material(std::string _name) :
	name(_name)
{
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
		auto texture = textures.find("material." + Texture::TypeNames.find((*i)->type)->second + "1");
		if (texture != textures.end()) {
			texture->second = (*i);
		}
		else {
			std::cout << "Error, unable to find texture spot for material:" << "material." + Texture::TypeNames.find((*i)->type)->second + "1\n";
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
	for (auto i = textures.begin(); i != textures.end(); i++)
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

void Material::GUI()
{
	std::string tag = PointerToString(this);
	ImGui::Text(name.c_str());
	ImGui::Text(("Shader ID:" + std::to_string(shader->GLID)).c_str());
	ImGui::Text("Textures");
	for (auto i = textures.begin(); i != textures.end(); i++)
	{
		ImGui::Text(i->first.c_str());
		
		//ImGui::InputInt((i->first + "##" + tag).c_str(), &i->s)

		if (i->second) {
			ImGui::SameLine();
			ImGui::Text(std::to_string(i->second->GLID).c_str()); // TODO: make this customisable
		}
	}
	for (auto i = floats.begin(); i != floats.end(); i++)
	{
		ImGui::Text(i->first.c_str());
		ImGui::DragFloat((i->first + "##" + tag).c_str(), &i->second);
	}
}
