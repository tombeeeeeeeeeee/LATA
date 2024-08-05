#include "Texture.h"

#include "Shader.h"

#include "Graphics.h"

#include "stb_image.h"
#include "imgui.h"
#include "imgui_stdlib.h"
#include "Utilities.h"

#include <iostream>

using Utilities::PointerToString;

const std::unordered_map<Texture::Type, std::string> Texture::TypeNames = 
{
	{ Type::diffuse, "diffuse" },
	{ Type::specular, "specular" },
	{ Type::normal, "normal" },
	{ Type::height, "height" },
	{ Type::emission, "emission" }, // TODO: Should cubemap be a type?
	{ Type::albedo, "albedo" },
	{ Type::PBR, "PBR" },
};

Texture::Texture(std::string _path, Type _type, int _wrappingMode, bool flip) :
	path(_path),
	type(_type),
	wrappingMode(_wrappingMode),
	flipped(flip)
{
	Load();
}

void Texture::Load(unsigned char* data)
{
	if (loaded) { DeleteTexture(); }
	if (path == "") {
		GLID = CreateTexture(width, height, format, data, wrappingMode, dataType, mipMapped, minFilter, maxFilter);
		loaded = true;
		return;
	}

	stbi_set_flip_vertically_on_load(flipped);

	int components;
	// Using data from something else
	if (data == nullptr) {
		data = stbi_load(path.c_str(), &width, &height, &components, STBI_default);
	}
	if (!data) {
		std::cout << "Texture failed to load, path: " << path << "\n";
		return;
	}

	GLenum format;
	switch (components)
	{
	case STBI_grey:
		format = GL_RED;
		break;
	case STBI_grey_alpha:
		format = GL_RG;
		break;
	case STBI_rgb:
		format = type != Type::diffuse && type != Type::albedo ? GL_RGB : GL_SRGB;
		break;
	case STBI_rgb_alpha:
		format = type != Type::diffuse && type != Type::albedo ? GL_RGBA : GL_SRGB_ALPHA;
		break;
	default:
		std::cout << "Texture failed to load, could not be read correctly, path: " << path << "\n";
		stbi_image_free(data);
		return; // Return instead of break
	}

	GLID = CreateTexture(width, height, format, data, wrappingMode, GL_UNSIGNED_BYTE, true, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
	stbi_image_free(data);
	loaded = true;
}

Texture::Texture(int _width, int _height, GLenum _format, unsigned char* data, GLint _wrappingMode, GLenum _dataType, bool _mipMaps, GLint _minFilter, GLint _magFilter) :
	width(_width),
	height(_height),
	format(_format),
	wrappingMode(_wrappingMode),
	dataType(_dataType),
	mipMapped(_mipMaps),
	minFilter(_minFilter),
	maxFilter(_magFilter)
{
	Load(data);
}

GLuint Texture::CreateTexture(int width, int height, GLenum format, unsigned char* data, GLint wrappingMode, GLenum dataType, bool mipMaps, GLint minFilter, GLint magFilter)
{
	GLuint ID;
	glGenTextures(1, &ID);

	glBindTexture(GL_TEXTURE_2D, ID);
	if(format == GL_SRGB_ALPHA)
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGBA, dataType, data);
	else if(format == GL_SRGB)
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGB, dataType, data);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, dataType, data);
	
	if (mipMaps) {
		glGenerateMipmap(GL_TEXTURE_2D); //TODO: Mip maps can look bad transitioning
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrappingMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrappingMode);

	return ID;
}

void Texture::setWidthHeight(int _width, int _height)
{
	width = _width;
	height = _height;
	Load();
}

void Texture::Bind(int indexToBindTo)
{
	int bindTo = indexToBindTo + GL_TEXTURE0;
	if (bindTo == GL_TEXTURE0) {
		std::cout << "Warning! Please don't bind to 0\n";
	}
	else if (bindTo > GL_TEXTURE31 || bindTo < GL_TEXTURE0) {
		std::cout << "Error! Trying to bind to invalid index of: " << indexToBindTo << "\nIndex must be within 0-31\n";
		return;
	}
	glActiveTexture(bindTo);
	glBindTexture(GL_TEXTURE_2D, GLID);
}

void Texture::ClearBind(int indexToClearBind)
{
	int bindTo = indexToClearBind + GL_TEXTURE0;
	if (bindTo > GL_TEXTURE31 || bindTo < GL_TEXTURE0) {
		std::cout << "Error! Trying to clear bind of invalid index: " << indexToClearBind << "\nIndex must be within 0-31\n";
		return;
	}
	glActiveTexture(bindTo);
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint Texture::LoadCubeMap(std::string faces[6])
{
	unsigned int GLID;
	glGenTextures(1, &GLID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, GLID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	int width, height, components;
	for (unsigned int i = 0; i < 6; i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &components, 0);
		if (!data) {
			std::cout << "Cubemap face failed to load at path: " << faces[i] << "\n";
			continue;
		}
		if(components == 3)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		else
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}
	return GLID;
}

void Texture::UseCubeMap(GLuint GLID, Shader* shader)
{
	shader->Use();
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, GLID);
	shader->setSampler("cubeMap", 1);
}

void Texture::DeleteTexture()
{
	glDeleteTextures(1, &GLID);
}

void Texture::EditorGUI(std::vector<Texture*>& textures) 
{
	if (ImGui::BeginTable("Resource Textures", 6)) {
		ImGui::TableNextRow();

		ImGui::TableSetColumnIndex(0);
		ImGui::Text("Path");

		ImGui::TableSetColumnIndex(1);
		ImGui::Text("Type");

		ImGui::TableSetColumnIndex(2);
		ImGui::Text("Flip");

		ImGui::TableSetColumnIndex(3);
		ImGui::Text("GUID");

		ImGui::TableSetColumnIndex(4);
		ImGui::Text("glID");

		ImGui::TableSetColumnIndex(5);
		ImGui::Text("Reload");

		for (auto i = textures.begin(); i != textures.end(); i++)
		{
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::PushItemWidth(270);
			ImGui::InputText(("##" + PointerToString(&(*i)->path)).c_str(), &(*i)->path); // TODO:

			ImGui::TableSetColumnIndex(1);

			ImGui::PushItemWidth(90);
			if (ImGui::BeginCombo(("##" + PointerToString(&(*i)->type)).c_str(), Texture::TypeNames.find((*i)->type)->second.c_str()))
			{
				for (int n = 0; n < (int)Texture::Type::count; n++)
				{
					const bool is_selected = ((int)(*i)->type == n);
					if (ImGui::Selectable(Texture::TypeNames.find((Texture::Type)n)->second.c_str(), is_selected)) {
						(*i)->type = (Texture::Type)n;
					}

					// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
					if (is_selected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			ImGui::TableSetColumnIndex(2);
			ImGui::Checkbox(("##" + PointerToString(&(*i)->flipped)).c_str(), &(*i)->flipped);

			ImGui::TableSetColumnIndex(3);
			ImGui::Text(std::to_string((*i)->GUID).c_str());

			ImGui::TableSetColumnIndex(4);
			ImGui::Text(std::to_string((*i)->GLID).c_str());

			ImGui::TableSetColumnIndex(5);
			if (ImGui::Button(("Reload##" + std::to_string((*i)->GLID)).c_str())) {
				(*i)->Load();
			}
		}
		ImGui::EndTable();
	}
}
