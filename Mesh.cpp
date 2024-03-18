#include "Mesh.h"

#include "assimp/scene.h"
#include "assimp/cimport.h"
#include <vector>

#include "glad.h"
#include "GLFW/glfw3.h"

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
}

void Mesh::Initialise(unsigned int vertexCount, const Vertex* vertices, unsigned int indexCount, unsigned int* indices)
{
	assert(VAO == 0);

	// generate buffers
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);

	// bind vertex array aka a mesh wrapper
	glBindVertexArray(VAO);

	// bind vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// fill vertex buffer
	glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex), vertices, GL_STATIC_DRAW);

	// enable first element as position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, sizeof(Vertex::position)/sizeof(Vertex::position.x), GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, sizeof(Vertex::normal) / sizeof(Vertex::normal.x), GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, sizeof(Vertex::texCoord) / sizeof(Vertex::texCoord.x), GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

	// bind indices if there are any
	if (indexCount > 0) {
		glGenBuffers(1, &IBO);

		// bind vertex buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

		// fill vertex buffer
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);

		triCount = indexCount / 3;
	}
	else {
		triCount = vertexCount / 3;
	}

	// unbind buffers
	Unbind();

}

void Mesh::InitialiseQuad()
{
	//TODO: Rewrite
	// Check that the mesh is not initialized already
	assert(VAO == 0);

	// Generate buffers
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);

	// Bind vertex array aka a mesh wrapper
	glBindVertexArray(VAO);

	// Bind vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Define 6 vertices for 2 triangles
	Vertex vertices[6] = {
		{ {  0.5f,  0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f, 0.0f }, { 1.0f, 0.0f } }, // Top right
		{ { -0.5f,  0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f } }, // Top left
		{ { -0.5f, -0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f } }, // Bottom left
		{ {  0.5f,  0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f, 0.0f }, { 1.0f, 0.0f } }, // Top right
		{ { -0.5f, -0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f } }, // Bottom left
		{ {  0.5f, -0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f } } // Bottom right
	};

	// Fill vertex buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Enable first element as position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, sizeof(Vertex::position) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	// unbind buffers
	Unbind();

	// quad has 2 triangles
	triCount = 2;
}
	
void Mesh::InitialiseCube()
{
	// check that the mesh is not initialized already
	assert(VAO == 0);

	// Generate buffers
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Bind vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Bind vertex array
	glBindVertexArray(VAO);

	Vertex vertices[6 * 6] = {
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

	// Fill vertex buffer 1440, 360
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Position
	glVertexAttribPointer(0, sizeof(Vertex::position) / sizeof(Vertex::position.x), GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);
	// Normal
	glVertexAttribPointer(1, sizeof(Vertex::normal) / sizeof(Vertex::normal.x), GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(1);
	// Texture
	glVertexAttribPointer(2, sizeof(Vertex::texCoord) / sizeof(Vertex::texCoord.x), GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
	glEnableVertexAttribArray(2);

	Unbind();

	triCount = 12;
}

void Mesh::InitialiseFromFile(const char* filename)
{
	const aiScene* scene = aiImportFile(filename, 0);
	// just use the first mesh we find for now
	// TODO: multiple meshes per file
	aiMesh* mesh = scene->mMeshes[0];


	// extract indicies from the first mesh
	int numFaces = mesh->mNumFaces;
	std::vector<unsigned int> indices;
	for (int i = 0; i < numFaces; i++)
	{
		for (int j = 0; j + 2 < mesh->mFaces[i].mNumIndices; j++)
		{
			indices.push_back(mesh->mFaces[i].mIndices[j]);
			indices.push_back(mesh->mFaces[i].mIndices[j+1]);
			indices.push_back(mesh->mFaces[i].mIndices[j+2]);
		}
	}

	// extract vertex data
	int numV = mesh->mNumVertices;
	Vertex* vertices = new Vertex[numV];
	for (int i = 0; i < numV; i++)
	{
		vertices[i].position = glm::vec4(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, 1);
		// TODO: normals and UVs
		if (mesh->HasNormals()) {
			vertices[i].normal = glm::vec4(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z, 0);
		}
		else {
			vertices[i].normal = glm::vec4(1, 0, 0, 0);
		}
		// TODO: all tex coords should be the same yeah
		if (mesh->HasTextureCoords(0)) {
			vertices[i].texCoord = glm::vec4(mesh->mTextureCoords[i]->x, mesh->mTextureCoords[i]->y, mesh->mTextureCoords[i]->z, 0);
		}
		else {
			vertices[i].texCoord = glm::vec2(0, 0);
		}
		
	}
	Initialise(numV, vertices, indices.size(), indices.data());
	delete[] vertices;
	aiReleaseImport(scene);
}

void Mesh::Draw(Shader& shader)
{
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;

	//for (unsigned int i = 0; i < textures.size(); i++)
	//{
	//	glActiveTexture(GL_TEXTURE0 + i);
	//	std::string number;
	//	std::string name = textures[i]->type;
	//	// TODO: bettr
	//	if (name == "diffuse") {
	//		number = std::to_string(diffuseNr++);
	//	}
	//	else if (name == "specular") {
	//		number = std::to_string(specularNr++);
	//	}

	//	shader.setFloat(("material." + name + number).c_str(), i);
	//	glBindTexture(GL_TEXTURE_2D, textures[i]->ID);
	//}
	//glActiveTexture(GL_TEXTURE0);

	glBindVertexArray(VAO);
	// using indices or just vertices?
	if (IBO != 0) {
		glDrawElements(GL_TRIANGLES, 3 * triCount, GL_UNSIGNED_INT, 0);
	}
	else {
		glDrawArrays(GL_TRIANGLES, 0, 3 * triCount);
	}
}

void Mesh::Unbind()
{
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Mesh::Vertex::Vertex(glm::vec4 pos, glm::vec4 nor, glm::vec2 tex) :
	position(pos),
	normal(nor),
	texCoord(tex)
{
}
