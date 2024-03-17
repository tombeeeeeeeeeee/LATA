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
	ibo(0)
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
	glVertexAttribPointer(0, sizeof(Vertex::position)/sizeof(float), GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, sizeof(Vertex::normal) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(void*)offsetof(Vertex, normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, sizeof(Vertex::texCoord) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(void*)offsetof(Vertex, texCoord));

	// bind indices if there are any
	if (indexCount > 0) {
		glGenBuffers(1, &ibo);

		// bind vertex buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

		// fill vertex buffer
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);

		triCount = indexCount / 3;
	}
	else {
		triCount = vertexCount / 3;
	}

	// unbind buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void Mesh::InitialiseQuad()
{
	//TODO: Rewrite
	// check that the mesh is not initialized already
	assert(VAO == 0);

	// generate buffers
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);

	// bind vertex array aka a mesh wrapper
	glBindVertexArray(VAO);

	// bind vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// define 6 vertices for 2 triangles
	Vertex vertices[6];
	vertices[0].position = { -0.5f, 0, 0.5f, 1 };
	vertices[1].position = { 0.5f, 0, 0.5f, 1 };
	vertices[2].position = { -0.5f, 0, -0.5f, 1 };

	vertices[3].position = { -0.5f, 0, -0.5f, 1 };
	vertices[4].position = { 0.5f, 0, 0.5f, 1 };
	vertices[5].position = { 0.5f, 0, -0.5f, 1 };

	// fill vertex buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// enable first element as position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

	// unbind buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

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

	//Vertex vertices[8];
	float vertices[6 * 6 * 9] = {
		// positions			   // normals           // texture coords
		-0.5f, -0.5f, -0.5f, 1.0f, 0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f, 1.0f, 0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f, 1.0f, 0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f, 1.0f, 0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f, 1.0f, 0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, 1.0f, 0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
							 
		-0.5f, -0.5f,  0.5f, 1.0f, 0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f, 1.0f, 0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f, 1.0f, 0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f, 1.0f, 0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f, 1.0f, 0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, 1.0f, 0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
							
		-0.5f,  0.5f,  0.5f, 1.0f,-1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, 1.0f,-1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, 1.0f,-1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, 1.0f,-1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, 1.0f,-1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, 1.0f,-1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
							
		 0.5f,  0.5f,  0.5f, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f, 1.0f, 1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f, 1.0f, 1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f, 1.0f, 1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
							 
		-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f, 1.0f, 0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f, 1.0f, 0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, 1.0f, 0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
							 
		-0.5f,  0.5f, -0.5f, 1.0f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f, 1.0f, 0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f, 1.0f, 0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f, 1.0f, 0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, 1.0f, 0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, 1.0f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};
	// Fill vertex buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Position
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// Normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(4 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// Texture
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(7 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// Unbind
	//TODO: is this really neccessary? still on the other inits
	//glBindVertexArray(0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	triCount = 12;
}

void Mesh::InitialiseFromFile(const char* filename)
{
	// read vertices from the model
	const aiScene* scene = aiImportFile(filename, 0);
	// just use the first mesh we find for now
	aiMesh* mesh = scene->mMeshes[0];


	// extract indicies from the first mesh
	int numFaces = mesh->mNumFaces;
	std::vector<unsigned int> indices;
	for (int i = 0; i < numFaces; i++)
	{
		indices.push_back(mesh->mFaces[i].mIndices[0]);
		indices.push_back(mesh->mFaces[i].mIndices[2]);
		indices.push_back(mesh->mFaces[i].mIndices[1]);
		// generate a second triangle for quads
		if (mesh->mFaces[i].mNumIndices == 4)
		{
			indices.push_back(mesh->mFaces[i].mIndices[0]);
			indices.push_back(mesh->mFaces[i].mIndices[3]);
			indices.push_back(mesh->mFaces[i].mIndices[2]);
		}
	}

	// extract vertex data
	int numV = mesh->mNumVertices;
	Vertex* vertices = new Vertex[numV];
	for (int i = 0; i < numV; i++)
	{
		vertices[i].position = glm::vec4(mesh->mVertices[i].x,
			mesh->mVertices[i].y, mesh->mVertices[i].z, 1);
		// TODO: normals and UVs
		//vertices[i].normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		
	}
	Initialise(numV, vertices, indices.size(), indices.data());
	delete[] vertices;
	aiReleaseImport(scene);
}

void Mesh::Draw()
{
	glBindVertexArray(VAO);
	// using indices or just vertices?
	if (ibo != 0) {
		glDrawElements(GL_TRIANGLES, 3 * triCount, GL_UNSIGNED_INT, 0);
	}
	else {
		glDrawArrays(GL_TRIANGLES, 0, 3 * triCount);
	}
}
