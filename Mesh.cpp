#include "Mesh.h"

#include "ResourceManager.h"

#include "Graphics.h"

#include <vector>
#include <iostream>
#include <unordered_map>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture*> _textures) :
	triCount(0),
	VAO(0),
	VBO(0),
	IBO(0),
	textures(_textures)
{
	// now that we have all the required data, set the vertex buffers and its attribute pointers.
	Initialise(vertices.size(), &vertices[0], indices.size(), &indices[0]);
}

Mesh::Mesh(unsigned int vertexCount, const Vertex* vertices, unsigned int indexCount, GLuint* indices) : Mesh()
{
	Initialise(vertexCount, vertices, indexCount, indices);
}

Mesh::Mesh():
	triCount(0),
	VAO(0),
	VBO(0),
	IBO(0)
{
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &IBO);
}

void Mesh::Initialise(unsigned int vertexCount, const Vertex* vertices, unsigned int indexCount, GLuint* indices)
{
	assert(VAO == 0);

	GenAndBind();

	// fill vertex buffer
	glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex), vertices, GL_STATIC_DRAW);

	Vertex::EnableAttributes();

	// bind indices if there are any
	if (indexCount > 0) {
		glGenBuffers(1, &IBO);

		// bind vertex buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

		// fill vertex buffer
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(GLuint), indices, GL_STATIC_DRAW);

		triCount = indexCount / 3;
	}
	else {
		triCount = vertexCount / 3;
	}

	// unbind buffers
	Unbind();
}

void Mesh::InitialiseFromAiMesh(std::string path, const aiScene* scene, aiMesh* mesh)
{
	unsigned int facesCount = mesh->mNumFaces;
	std::vector<GLuint> indices;
	for (unsigned int i = 0; i < facesCount; i++)
	{
		for (unsigned int j = 0; j + 2 < mesh->mFaces[i].mNumIndices; j++)
		{
			indices.push_back(mesh->mFaces[i].mIndices[j]);
			indices.push_back(mesh->mFaces[i].mIndices[j + 1]);
			indices.push_back(mesh->mFaces[i].mIndices[j + 2]);
		}
	}

	// extract vertex data
	int vertexCount = mesh->mNumVertices;
	Vertex* vertices = new Vertex[vertexCount];
	for (int i = 0; i < vertexCount; i++)
	{
		vertices[i].position = glm::vec4(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, 1.f);
		if (mesh->HasNormals()) {
			vertices[i].normal = glm::vec4(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z, 0.f);
		}
		else {
			vertices[i].normal = glm::vec4(1.f, 0.f, 0.f, 0.f);
		}
		// TODO: other tex coords?
		if (mesh->mTextureCoords[0]) {
			vertices[i].texCoord = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		}
		else {
			vertices[i].texCoord = glm::vec2(0.0f, 0.0f);
		}
	}

	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	//TODO: This shouldn't have to be changed for when another texture type is supported
	AddMaterialTextures(path, material, aiTextureType_DIFFUSE, Texture::Type::diffuse);
	AddMaterialTextures(path, material, aiTextureType_SPECULAR, Texture::Type::specular);
	AddMaterialTextures(path, material, aiTextureType_NORMALS, Texture::Type::normal);
	
	Initialise(vertexCount, vertices, indices.size(), indices.data());
	delete[] vertices;
}

void Mesh::InitialiseQuad()
{
	const unsigned int vertexCount = 6;
	Vertex vertices[vertexCount] = {
		{ {  0.5f,  0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f,  1.0f, 0.0f }, { 1.0f, 0.0f } }, // Top right
		{ { -0.5f,  0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f,  1.0f, 0.0f }, { 0.0f, 0.0f } }, // Top left
		{ { -0.5f, -0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f,  1.0f, 0.0f }, { 0.0f, 1.0f } }, // Bottom left
		{ {  0.5f,  0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f,  1.0f, 0.0f }, { 1.0f, 0.0f } }, // Top right
		{ { -0.5f, -0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f,  1.0f, 0.0f }, { 0.0f, 1.0f } }, // Bottom left
		{ {  0.5f, -0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f,  1.0f, 0.0f }, { 1.0f, 1.0f } } // Bottom right
	};
	Initialise(vertexCount, vertices);
}

void Mesh::InitialiseDoubleSidedQuad()
{
	const unsigned int vertexCount = 12;
	Vertex vertices[vertexCount] = {
		{ {  0.5f,  0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f,  1.0f, 0.0f }, { 1.0f, 0.0f } }, // Top right
		{ { -0.5f,  0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f,  1.0f, 0.0f }, { 0.0f, 0.0f } }, // Top left
		{ { -0.5f, -0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f,  1.0f, 0.0f }, { 0.0f, 1.0f } }, // Bottom left
		{ {  0.5f,  0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f,  1.0f, 0.0f }, { 1.0f, 0.0f } }, // Top right
		{ { -0.5f, -0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f,  1.0f, 0.0f }, { 0.0f, 1.0f } }, // Bottom left
		{ {  0.5f, -0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f,  1.0f, 0.0f }, { 1.0f, 1.0f } }, // Bottom right

		{ {  0.5f,  0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f, 0.0f }, { 1.0f, 0.0f } }, // Top right
		{ { -0.5f, -0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f } }, // Bottom left
		{ { -0.5f,  0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f } }, // Top left
		{ {  0.5f,  0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f, 0.0f }, { 1.0f, 0.0f } }, // Top right
		{ {  0.5f, -0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f, 0.0f }, { 1.0f, 1.0f } }, // Bottom right
		{ { -0.5f, -0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f } } // Bottom left
	};
	Initialise(vertexCount, vertices);
}
	
void Mesh::InitialiseCube()
{
	const unsigned int vertexCount = 36;
	Vertex vertices[vertexCount] = {
		// positions				 // normals           // texture coords
		{ {-0.5f, -0.5f, -0.5f, 1.0f},	{ 0.0f,  0.0f, -1.0f, 0.0f},  {0.0f,  0.0f} },
		{ { 0.5f,  0.5f, -0.5f, 1.0f},	{ 0.0f,  0.0f, -1.0f, 0.0f},  {1.0f,  1.0f} },
		{ { 0.5f, -0.5f, -0.5f, 1.0f},	{ 0.0f,  0.0f, -1.0f, 0.0f},  {1.0f,  0.0f} },
		{ { 0.5f,  0.5f, -0.5f, 1.0f},	{ 0.0f,  0.0f, -1.0f, 0.0f},  {1.0f,  1.0f} },
		{ {-0.5f, -0.5f, -0.5f, 1.0f},	{ 0.0f,  0.0f, -1.0f, 0.0f},  {0.0f,  0.0f} },
		{ {-0.5f,  0.5f, -0.5f, 1.0f},	{ 0.0f,  0.0f, -1.0f, 0.0f},  {0.0f,  1.0f} },
		   // front face                                                                                    
		{ {-0.5f, -0.5f,  0.5f, 1.0f},	{ 0.0f,  0.0f,  1.0f, 0.0f},  {0.0f,  0.0f} },
		{ { 0.5f, -0.5f,  0.5f, 1.0f},	{ 0.0f,  0.0f,  1.0f, 0.0f},  {1.0f,  0.0f} },
		{ { 0.5f,  0.5f,  0.5f, 1.0f},	{ 0.0f,  0.0f,  1.0f, 0.0f},  {1.0f,  1.0f} },
		{ { 0.5f,  0.5f,  0.5f, 1.0f},	{ 0.0f,  0.0f,  1.0f, 0.0f},  {1.0f,  1.0f} },
		{ {-0.5f,  0.5f,  0.5f, 1.0f},	{ 0.0f,  0.0f,  1.0f, 0.0f},  {0.0f,  1.0f} },
		{ {-0.5f, -0.5f,  0.5f, 1.0f},	{ 0.0f,  0.0f,  1.0f, 0.0f},  {0.0f,  0.0f} },
		   // left face                                                                                                                                                            
		{ {-0.5f,  0.5f,  0.5f, 1.0f},	{-1.0f,  0.0f,  0.0f, 0.0f},  {1.0f,  0.0f} },
		{ {-0.5f,  0.5f, -0.5f, 1.0f},	{-1.0f,  0.0f,  0.0f, 0.0f},  {1.0f,  1.0f} },
		{ {-0.5f, -0.5f, -0.5f, 1.0f},	{-1.0f,  0.0f,  0.0f, 0.0f},  {0.0f,  1.0f} },
		{ {-0.5f, -0.5f, -0.5f, 1.0f},	{-1.0f,  0.0f,  0.0f, 0.0f},  {0.0f,  1.0f} },
		{ {-0.5f, -0.5f,  0.5f, 1.0f},	{-1.0f,  0.0f,  0.0f, 0.0f},  {0.0f,  0.0f} },
		{ {-0.5f,  0.5f,  0.5f, 1.0f},	{-1.0f,  0.0f,  0.0f, 0.0f},  {1.0f,  0.0f} },
		   // right face                                                                                     
		{ { 0.5f,  0.5f,  0.5f, 1.0f},	{ 1.0f,  0.0f,  0.0f, 0.0f},  {1.0f,  0.0f} },
		{ { 0.5f, -0.5f, -0.5f, 1.0f},	{ 1.0f,  0.0f,  0.0f, 0.0f},  {0.0f,  1.0f} },
		{ { 0.5f,  0.5f, -0.5f, 1.0f},	{ 1.0f,  0.0f,  0.0f, 0.0f},  {1.0f,  1.0f} },
		{ { 0.5f, -0.5f, -0.5f, 1.0f},	{ 1.0f,  0.0f,  0.0f, 0.0f},  {0.0f,  1.0f} },
		{ { 0.5f,  0.5f,  0.5f, 1.0f},	{ 1.0f,  0.0f,  0.0f, 0.0f},  {1.0f,  0.0f} },
		{ { 0.5f, -0.5f,  0.5f, 1.0f},	{ 1.0f,  0.0f,  0.0f, 0.0f},  {0.0f,  0.0f} },
		   // bottom face                                                                                         
		{ {-0.5f, -0.5f, -0.5f, 1.0f},	{ 0.0f, -1.0f,  0.0f, 0.0f},  {0.0f,  1.0f} },
		{ { 0.5f, -0.5f, -0.5f, 1.0f},	{ 0.0f, -1.0f,  0.0f, 0.0f},  {1.0f,  1.0f} },
		{ { 0.5f, -0.5f,  0.5f, 1.0f},	{ 0.0f, -1.0f,  0.0f, 0.0f},  {1.0f,  0.0f} },
		{ { 0.5f, -0.5f,  0.5f, 1.0f},	{ 0.0f, -1.0f,  0.0f, 0.0f},  {1.0f,  0.0f} },
		{ {-0.5f, -0.5f,  0.5f, 1.0f},	{ 0.0f, -1.0f,  0.0f, 0.0f},  {0.0f,  0.0f} },
		{ {-0.5f, -0.5f, -0.5f, 1.0f},	{ 0.0f, -1.0f,  0.0f, 0.0f},  {0.0f,  1.0f} },
		   // top face                                                                                    
		{ {-0.5f,  0.5f, -0.5f, 1.0f},	{ 0.0f,  1.0f,  0.0f, 0.0f},  {0.0f,  1.0f} },
		{ { 0.5f,  0.5f,  0.5f, 1.0f},	{ 0.0f,  1.0f,  0.0f, 0.0f},  {1.0f,  0.0f} },
		{ { 0.5f,  0.5f, -0.5f, 1.0f},	{ 0.0f,  1.0f,  0.0f, 0.0f},  {1.0f,  1.0f} },
		{ { 0.5f,  0.5f,  0.5f, 1.0f},	{ 0.0f,  1.0f,  0.0f, 0.0f},  {1.0f,  0.0f} },
		{ {-0.5f,  0.5f, -0.5f, 1.0f},	{ 0.0f,  1.0f,  0.0f, 0.0f},  {0.0f,  1.0f} },
		{ {-0.5f,  0.5f,  0.5f, 1.0f},	{ 0.0f,  1.0f,  0.0f, 0.0f},  {0.0f,  0.0f} },
	};                                                                                    
	Initialise(vertexCount, vertices);
}

void Mesh::InitialiseFromFile(std::string filename)
{
	InitialiseIndexFromFile(filename.c_str(), 0);
}

void Mesh::InitialiseIndexFromFile(std::string path, int i)
{
	const aiScene* scene = aiImportFile(path.c_str(), 0);
	if (!scene) {
		std::cout << "Mesh failed to load at: " << path << "\n";
		return;
	}
	aiMesh* mesh = scene->mMeshes[i];

	InitialiseFromAiMesh(path, scene, mesh);

	aiReleaseImport(scene);
}

void Mesh::Draw(Shader* shader)
{
	shader->Use();

	// bind appropriate textures
	std::unordered_map<Texture::Type, unsigned int> typeCounts;
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i+1);
		unsigned int number;
		std::string name = Texture::TypeNames.find(textures[i]->type)->second;
		// Get the amount of this type of texture has been assigned
		auto typeCount = typeCounts.find(textures[i]->type);
		if (typeCount == typeCounts.end()) {
			//TODO: Should this be emplace or insert
			typeCount = typeCounts.emplace(std::pair<Texture::Type, unsigned int>(textures[i]->type, 1)).first;
		}
		else {
			typeCount->second++;
		}
		number = typeCount->second;

		// now set the sampler to the correct texture unit
		shader->setSampler(("material." + name + std::to_string(number)), i+1);

		// and finally bind the texture
		glBindTexture(GL_TEXTURE_2D, textures[i]->ID);
	}

	// draw mesh
	glBindVertexArray(VAO);
	if (IBO != 0) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glDrawElements(GL_TRIANGLES, 3 * triCount, GL_UNSIGNED_INT, 0);
	}
	else {
		glDrawArrays(GL_TRIANGLES, 0, 3 * triCount);
	}

	Unbind();
	
	// TODO: how many times should this loops, currently five as there shouldn't be more than 5 active textures so this should be okay but it shouldn't be constant as this could later change in the future
	// Unbinds any assigned textures, this is so if a mesh only has one diffuse, the previously set specular from another mesh isn't used.
	for (unsigned int i = 0; i < 5; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glActiveTexture(GL_TEXTURE0);
}

void Mesh::GenAndBind()
{
	// Generate buffers
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Bind vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Bind vertex array
	glBindVertexArray(VAO);
}

void Mesh::Unbind()
{
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// TODO: These should specifically be called for files found in the mtl file if there
std::vector<Texture*> Mesh::LoadMaterialTextures(std::string path, aiMaterial* mat, aiTextureType aiType, Texture::Type type)
{
	std::vector<Texture*> textures;
	textures.reserve(mat->GetTextureCount(aiType));
	for (unsigned int i = 0; i < mat->GetTextureCount(aiType); i++)
	{
		aiString str;
		mat->GetTexture(aiType, i, &str);
		std::string folder = path.substr(0, 1 + path.find_last_of("\\/"));
		Texture* texture = ResourceManager::GetTexture(folder + str.C_Str(), type);
		if (texture) {
			texture->type = type;
			textures.push_back(texture);
		}
		else {
			std::cout << "Failed attempt to find a texture of type: " << str.C_Str() << "\n";
		}
	}
	return textures;
}

void Mesh::AddMaterialTextures(std::string path, aiMaterial* mat, aiTextureType aiType, Texture::Type type)
{
	std::vector<Texture*> maps = LoadMaterialTextures(path, mat, aiType, type);
	textures.insert(textures.end(), maps.begin(), maps.end());
}