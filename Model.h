#pragma once

#include "Maths.h"
#include "Shader.h"
#include <vector>
#include "Mesh.h"

#include "glad.h"
#include "GLFW/glfw3.h"
#include "assimp/scene.h"
#include <assimp/Importer.hpp>
#include "assimp/postprocess.h"


class Model
{
public:
	Model() = default;
	Model(std::string path);
	void Draw(Shader& shader);
private:
	// model data
	std::vector<Mesh> meshes;
	std::string directory;
	void LoadModel(std::string path);
	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture*> LoadMaterialTextures(aiMaterial* mat, aiTextureType aiType, Texture::Type type);
};

