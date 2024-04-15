#include "Mesh.h"

#include "ResourceManager.h"
#include "Model.h"

#include "AssimpMatrixToGLM.h"

#include "Graphics.h"

#include <vector>
#include <iostream>
#include <unordered_map>

//TODO: Look into more assimp load flags
int Mesh::aiLoadFlag = aiProcess_CalcTangentSpace /*| aiProcess_JoinIdenticalVertices | aiProcess_Triangulate*/;

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices) :
	triCount(0),
	VAO(0),
	VBO(0),
	IBO(0)
{
	// now that we have all the required data, set the vertex buffers and its attribute pointers.
	Initialise((int)vertices.size(), &vertices[0], (int)indices.size(), &indices[0]);
}

Mesh::Mesh(unsigned int vertexCount, const Vertex* vertices, unsigned int indexCount, GLuint* indices) : Mesh()
{
	Initialise(vertexCount, vertices, indexCount, indices);
}

Mesh::Mesh(presets preset) :
	triCount(0),
	VAO(0),
	VBO(0),
	IBO(0)
{
	switch (preset)
	{
	case Mesh::presets::cube:
		InitialiseCube();
		break;
	case Mesh::presets::quad:
		InitialiseQuad();
		break;
	case Mesh::presets::doubleQuad:
		InitialiseDoubleSidedQuad();
		break;
	case Mesh::presets::cubeOppositeWind:
		InitialiseCubeInsideOut();
		break;
	default:
		std::cout << "Error, unsuported preset attempted to create on creation\n";
		break;
	}
}

Mesh::Mesh() :
	triCount(0),
	VAO(0),
	VBO(0),
	IBO(0)
{
}

Mesh::~Mesh()
{
	//std::cout << "A mesh was destroyed\n";
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &IBO);
}

Mesh::Mesh(Mesh&& other) noexcept
{
	//std::cout << "A mesh was moved via constructor!\n";
	triCount = other.triCount;
	VAO = other.VAO;
	VBO = other.VBO;
	IBO = other.IBO;
	
	other.triCount = 0;
	other.VAO = 0;
	other.VBO = 0;
	other.IBO = 0;
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

void Mesh::InitialiseFromAiMesh(std::string path, const aiScene* scene, std::unordered_map<std::string, BoneInfo>* boneInfos, aiMesh* mesh, bool flipTexturesOnLoad)
{
	unsigned int facesCount = mesh->mNumFaces;
	std::vector<GLuint> indices;
	for (unsigned int i = 0; i < facesCount; i++)
	{
		for (unsigned int j = 0; j + 2 < mesh->mFaces[i].mNumIndices; j++)
		{
			indices.push_back(mesh->mFaces[i].mIndices[0]);
			indices.push_back(mesh->mFaces[i].mIndices[j + 1]);
			indices.push_back(mesh->mFaces[i].mIndices[j + 2]);
		}
	}

	// extract vertex data
	int vertexCount = mesh->mNumVertices;
	Vertex* vertices = new Vertex[vertexCount];
	for (int i = 0; i < vertexCount; i++)
	{
		vertices[i].position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, 1.f };
		if (mesh->HasNormals()) {
			vertices[i].normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z, 0.f };
		}
		else {
			vertices[i].normal = { 1.f, 0.f, 0.f, 0.f };
		}
		// TODO: other tex coords?
		if (mesh->mTextureCoords[0]) {
			vertices[i].texCoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
		}
		else {
			vertices[i].texCoord = { 0.0f, 0.0f };
		}
		if (mesh->HasTangentsAndBitangents()) {
			vertices[i].tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
			vertices[i].biTangent = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
		}
		else {
			vertices[i].tangent = { 0, 0, 0 };
			vertices[i].biTangent = { 0, 0, 0 };
		}
	}
	for (int boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++)
	{
		aiBone* bone = mesh->mBones[boneIndex];
		std::string boneName = bone->mName.C_Str();

		auto boneInfo = boneInfos->find(boneName);
		if (boneInfo == boneInfos->end()) {
			BoneInfo newBoneInfo;
			newBoneInfo.id = boneInfos->size();
			newBoneInfo.offset = AssimpMatrixToGLM(bone->mOffsetMatrix);
			boneInfo = boneInfos->emplace(boneName, newBoneInfo).first;
		}
		int boneID = boneInfo->second.id;
		
		for (int weightIndex = 0; weightIndex < bone->mNumWeights; weightIndex++)
		{
			int vertexIndex = bone->mWeights[weightIndex].mVertexId;
			int weight = bone->mWeights[weightIndex].mWeight;
			Vertex* vertex = &vertices[vertexIndex];
			
			// Find empty bone/weight spot
			for (int i = 0; i < MAX_BONES_PER_VERTEX; ++i)
			{
				if (vertex->boneIDs[i] >= 0) { continue; }
				vertex->boneIDs[i] = boneID;
				vertex->weights[i] = weight;
				break;
			}
		}

	}

	// TODO: Move somewhere else
	//Texture stuff
	//aiMaterial* ai_material = scene->mMaterials[mesh->mMaterialIndex];

	//Texture textures[] = {
	//std::vector<Texture*> textures;
	//TODO: This shouldn't have to be changed for when another texture type is supported, make some sorta loop or dynamically check what materials there are
	//AddMaterialTextures(&textures, path, ai_material, aiTextureType_DIFFUSE, Texture::Type::diffuse, flipTexturesOnLoad);
	//AddMaterialTextures(&textures, path, ai_material, aiTextureType_SPECULAR, Texture::Type::specular, flipTexturesOnLoad);
	//AddMaterialTextures(&textures, path, ai_material, aiTextureType_NORMALS, Texture::Type::normal, flipTexturesOnLoad); // TODO: are the normals actually stored in aiTextureType_HEIGHT for obj files?

	//material = ResourceManager::GetMaterial(textures);

	Initialise(vertexCount, vertices, (int)indices.size(), indices.data());
	delete[] vertices;
}

void Mesh::InitialiseQuad()
{
	// TODO: Fix pre set shapes
	//const unsigned int vertexCount = 6;
	//Vertex vertices[vertexCount] = {
	//	{ {  0.5f,  0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f,  1.0f, 0.0f }, { 1.0f, 0.0f } }, // Top right
	//	{ { -0.5f,  0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f,  1.0f, 0.0f }, { 0.0f, 0.0f } }, // Top left
	//	{ { -0.5f, -0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f,  1.0f, 0.0f }, { 0.0f, 1.0f } }, // Bottom left
	//	{ {  0.5f,  0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f,  1.0f, 0.0f }, { 1.0f, 0.0f } }, // Top right
	//	{ { -0.5f, -0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f,  1.0f, 0.0f }, { 0.0f, 1.0f } }, // Bottom left
	//	{ {  0.5f, -0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f,  1.0f, 0.0f }, { 1.0f, 1.0f } } // Bottom right
	//};
	//Initialise(vertexCount, vertices);
}

// TODO: Maybe a function that initialises a shape as double sided instead of a specific version for each
void Mesh::InitialiseDoubleSidedQuad()
{
	const unsigned int vertexCount = 12;
	Vertex vertices[vertexCount] = {
		{ {  0.5f,  0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f,  1.0f, 0.0f }, { 1.0f, 0.0f }, { 0, 1, 0 }, { 0, 1, 0 } }, // Top right
		{ { -0.5f,  0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f,  1.0f, 0.0f }, { 0.0f, 0.0f }, { 0, 1, 0 }, { 0, 1, 0 } }, // Top left
		{ { -0.5f, -0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f,  1.0f, 0.0f }, { 0.0f, 1.0f }, { 0, 1, 0 }, { 0, 1, 0 } }, // Bottom left
		{ {  0.5f,  0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f,  1.0f, 0.0f }, { 1.0f, 0.0f }, { 0, 1, 0 }, { 0, 1, 0 } }, // Top right
		{ { -0.5f, -0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f,  1.0f, 0.0f }, { 0.0f, 1.0f }, { 0, 1, 0 }, { 0, 1, 0 } }, // Bottom left
		{ {  0.5f, -0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f,  1.0f, 0.0f }, { 1.0f, 1.0f }, { 0, 1, 0 }, { 0, 1, 0 } }, // Bottom right

		{ {  0.5f,  0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f, 0.0f }, { 1.0f, 0.0f }, { 0, 1, 0 }, { 0, 1, 0 } }, // Top right
		{ { -0.5f, -0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f }, { 0, 1, 0 }, { 0, 1, 0 } }, // Bottom left
		{ { -0.5f,  0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f }, { 0, 1, 0 }, { 0, 1, 0 } }, // Top left
		{ {  0.5f,  0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f, 0.0f }, { 1.0f, 0.0f }, { 0, 1, 0 }, { 0, 1, 0 } }, // Top right
		{ {  0.5f, -0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f, 0.0f }, { 1.0f, 1.0f }, { 0, 1, 0 }, { 0, 1, 0 } }, // Bottom right
		{ { -0.5f, -0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f }, { 0, 1, 0 }, { 0, 1, 0 } } // Bottom left
	};
	Initialise(vertexCount, vertices);
}

void Mesh::InitialiseCube()
{
	const unsigned int vertexCount = 36;
	Vertex vertices[vertexCount] = {
		// positions					// normals					  // texture coords
		{ {-0.5f, -0.5f, -0.5f, 1.0f},	{ 0.0f,  0.0f, -1.0f, 0.0f},  {0.0f,  0.0f}, { 0, 1, 0 }, { 1, 0, 0 } },
		{ { 0.5f,  0.5f, -0.5f, 1.0f},	{ 0.0f,  0.0f, -1.0f, 0.0f},  {1.0f,  1.0f}, { 0, 1, 0 }, { 1, 0, 0 } },
		{ { 0.5f, -0.5f, -0.5f, 1.0f},	{ 0.0f,  0.0f, -1.0f, 0.0f},  {1.0f,  0.0f}, { 0, 1, 0 }, { 1, 0, 0 } }, //
		{ { 0.5f,  0.5f, -0.5f, 1.0f},	{ 0.0f,  0.0f, -1.0f, 0.0f},  {1.0f,  1.0f}, { 0, 1, 0 }, { 1, 0, 0 } },
		{ {-0.5f, -0.5f, -0.5f, 1.0f},	{ 0.0f,  0.0f, -1.0f, 0.0f},  {0.0f,  0.0f}, { 0, 1, 0 }, { 1, 0, 0 } },
		{ {-0.5f,  0.5f, -0.5f, 1.0f},	{ 0.0f,  0.0f, -1.0f, 0.0f},  {0.0f,  1.0f}, { 0, 1, 0 }, { 1, 0, 0 } },
		// front face                                                                             
		{ {-0.5f, -0.5f,  0.5f, 1.0f},	{ 0.0f,  0.0f,  1.0f, 0.0f},  {0.0f,  0.0f}, { 0, 1, 0 }, {-1, 0, 0 } },
		{ { 0.5f, -0.5f,  0.5f, 1.0f},	{ 0.0f,  0.0f,  1.0f, 0.0f},  {1.0f,  0.0f}, { 0, 1, 0 }, {-1, 0, 0 } },
		{ { 0.5f,  0.5f,  0.5f, 1.0f},	{ 0.0f,  0.0f,  1.0f, 0.0f},  {1.0f,  1.0f}, { 0, 1, 0 }, {-1, 0, 0 } },
		{ { 0.5f,  0.5f,  0.5f, 1.0f},	{ 0.0f,  0.0f,  1.0f, 0.0f},  {1.0f,  1.0f}, { 0, 1, 0 }, {-1, 0, 0 } }, //
		{ {-0.5f,  0.5f,  0.5f, 1.0f},	{ 0.0f,  0.0f,  1.0f, 0.0f},  {0.0f,  1.0f}, { 0, 1, 0 }, {-1, 0, 0 } },
		{ {-0.5f, -0.5f,  0.5f, 1.0f},	{ 0.0f,  0.0f,  1.0f, 0.0f},  {0.0f,  0.0f}, { 0, 1, 0 }, {-1, 0, 0 } },
		// left face                         							                                                             		   
		{ {-0.5f,  0.5f,  0.5f, 1.0f},	{-1.0f,  0.0f,  0.0f, 0.0f},  {1.0f,  0.0f}, { 0, 1, 0 }, { 0, 0,-1 } },
		{ {-0.5f,  0.5f, -0.5f, 1.0f},	{-1.0f,  0.0f,  0.0f, 0.0f},  {1.0f,  1.0f}, { 0, 1, 0 }, { 0, 0,-1 } },
		{ {-0.5f, -0.5f, -0.5f, 1.0f},	{-1.0f,  0.0f,  0.0f, 0.0f},  {0.0f,  1.0f}, { 0, 1, 0 }, { 0, 0,-1 } }, //
		{ {-0.5f, -0.5f, -0.5f, 1.0f},	{-1.0f,  0.0f,  0.0f, 0.0f},  {0.0f,  1.0f}, { 0, 1, 0 }, { 0, 0,-1 } },
		{ {-0.5f, -0.5f,  0.5f, 1.0f},	{-1.0f,  0.0f,  0.0f, 0.0f},  {0.0f,  0.0f}, { 0, 1, 0 }, { 0, 0,-1 } },
		{ {-0.5f,  0.5f,  0.5f, 1.0f},	{-1.0f,  0.0f,  0.0f, 0.0f},  {1.0f,  0.0f}, { 0, 1, 0 }, { 0, 0,-1 } },
		// right face                                                                                             
		{ { 0.5f,  0.5f,  0.5f, 1.0f},	{ 1.0f,  0.0f,  0.0f, 0.0f},  {1.0f,  0.0f}, { 0, 1, 0 }, { 0, 0, 1 } },
		{ { 0.5f, -0.5f, -0.5f, 1.0f},	{ 1.0f,  0.0f,  0.0f, 0.0f},  {0.0f,  1.0f}, { 0, 1, 0 }, { 0, 0, 1 } },
		{ { 0.5f,  0.5f, -0.5f, 1.0f},	{ 1.0f,  0.0f,  0.0f, 0.0f},  {1.0f,  1.0f}, { 0, 1, 0 }, { 0, 0, 1 } }, //
		{ { 0.5f, -0.5f, -0.5f, 1.0f},	{ 1.0f,  0.0f,  0.0f, 0.0f},  {0.0f,  1.0f}, { 0, 1, 0 }, { 0, 0, 1 } },
		{ { 0.5f,  0.5f,  0.5f, 1.0f},	{ 1.0f,  0.0f,  0.0f, 0.0f},  {1.0f,  0.0f}, { 0, 1, 0 }, { 0, 0, 1 } },
		{ { 0.5f, -0.5f,  0.5f, 1.0f},	{ 1.0f,  0.0f,  0.0f, 0.0f},  {0.0f,  0.0f}, { 0, 1, 0 }, { 0, 0, 1 } },
		// bottom face                                                                                                
		{ {-0.5f, -0.5f, -0.5f, 1.0f},	{ 0.0f, -1.0f,  0.0f, 0.0f},  {0.0f,  1.0f}, { 1, 0, 0 }, { 0, 0, 1 } },
		{ { 0.5f, -0.5f, -0.5f, 1.0f},	{ 0.0f, -1.0f,  0.0f, 0.0f},  {1.0f,  1.0f}, { 1, 0, 0 }, { 0, 0, 1 } },
		{ { 0.5f, -0.5f,  0.5f, 1.0f},	{ 0.0f, -1.0f,  0.0f, 0.0f},  {1.0f,  0.0f}, { 1, 0, 0 }, { 0, 0, 1 } }, //
		{ { 0.5f, -0.5f,  0.5f, 1.0f},	{ 0.0f, -1.0f,  0.0f, 0.0f},  {1.0f,  0.0f}, { 1, 0, 0 }, { 0, 0, 1 } },
		{ {-0.5f, -0.5f,  0.5f, 1.0f},	{ 0.0f, -1.0f,  0.0f, 0.0f},  {0.0f,  0.0f}, { 1, 0, 0 }, { 0, 0, 1 } },
		{ {-0.5f, -0.5f, -0.5f, 1.0f},	{ 0.0f, -1.0f,  0.0f, 0.0f},  {0.0f,  1.0f}, { 1, 0, 0 }, { 0, 0, 1 } },
		// top face                                                                                               
		{ {-0.5f,  0.5f, -0.5f, 1.0f},	{ 0.0f,  1.0f,  0.0f, 0.0f},  {0.0f,  1.0f}, {-1, 0, 0 }, { 0, 0, 1 } },
		{ { 0.5f,  0.5f,  0.5f, 1.0f},	{ 0.0f,  1.0f,  0.0f, 0.0f},  {1.0f,  0.0f}, {-1, 0, 0 }, { 0, 0, 1 } },
		{ { 0.5f,  0.5f, -0.5f, 1.0f},	{ 0.0f,  1.0f,  0.0f, 0.0f},  {1.0f,  1.0f}, {-1, 0, 0 }, { 0, 0, 1 } }, //
		{ { 0.5f,  0.5f,  0.5f, 1.0f},	{ 0.0f,  1.0f,  0.0f, 0.0f},  {1.0f,  0.0f}, {-1, 0, 0 }, { 0, 0, 1 } },
		{ {-0.5f,  0.5f, -0.5f, 1.0f},	{ 0.0f,  1.0f,  0.0f, 0.0f},  {0.0f,  1.0f}, {-1, 0, 0 }, { 0, 0, 1 } },
		{ {-0.5f,  0.5f,  0.5f, 1.0f},	{ 0.0f,  1.0f,  0.0f, 0.0f},  {0.0f,  0.0f}, {-1, 0, 0 }, { 0, 0, 1 } },
	};
	Initialise(vertexCount, vertices);
}

void Mesh::InitialiseCubeInsideOut() // TODO: Name this more clear
{
	const unsigned int vertexCount = 36;
	Vertex vertices[vertexCount] = {
		// positions					
		{ {-1.0f, -1.0f, -1.0f, 1.0f} },
		{ { 1.0f, -1.0f, -1.0f, 1.0f} }, 
		{ { 1.0f,  1.0f, -1.0f, 1.0f} },
		{ { 1.0f,  1.0f, -1.0f, 1.0f} },
		{ {-1.0f,  1.0f, -1.0f, 1.0f} },
		{ {-1.0f, -1.0f, -1.0f, 1.0f} },
		// left face                                         		   
		{ {-1.0f,  1.0f,  1.0f, 1.0f} },
		{ {-1.0f, -1.0f, -1.0f, 1.0f} }, 
		{ {-1.0f,  1.0f, -1.0f, 1.0f} },
		{ {-1.0f, -1.0f, -1.0f, 1.0f} },
		{ {-1.0f,  1.0f,  1.0f, 1.0f} },
		{ {-1.0f, -1.0f,  1.0f, 1.0f} },
		// right face                     
		{ { 1.0f,  1.0f,  1.0f, 1.0f} },
		{ { 1.0f,  1.0f, -1.0f, 1.0f} }, 
		{ { 1.0f, -1.0f, -1.0f, 1.0f} },
		{ { 1.0f, -1.0f, -1.0f, 1.0f} },
		{ { 1.0f, -1.0f,  1.0f, 1.0f} },
		{ { 1.0f,  1.0f,  1.0f, 1.0f} },
		// front face                 		
		{ { -1.0f,-1.0f,  1.0f, 1.0f} },
		{ { 1.0f,  1.0f,  1.0f, 1.0f} },
		{ { 1.0f, -1.0f,  1.0f, 1.0f} },
		{ { 1.0f,  1.0f,  1.0f, 1.0f} }, 
		{ {-1.0f, -1.0f,  1.0f, 1.0f} },
		{ {-1.0f,  1.0f,  1.0f, 1.0f} },
		// top face                       
		{ {-1.0f,  1.0f, -1.0f, 1.0f} },
		{ { 1.0f,  1.0f, -1.0f, 1.0f} }, 
		{ { 1.0f,  1.0f,  1.0f, 1.0f} },
		{ { 1.0f,  1.0f,  1.0f, 1.0f} },
		{ {-1.0f,  1.0f,  1.0f, 1.0f} },
		{ {-1.0f,  1.0f, -1.0f, 1.0f} },
		// bottom face                        
		{ {-1.0f, -1.0f, -1.0f, 1.0f} },
		{ { 1.0f, -1.0f,  1.0f, 1.0f} }, 
		{ { 1.0f, -1.0f, -1.0f, 1.0f} },
		{ { 1.0f, -1.0f,  1.0f, 1.0f} },
		{ {-1.0f, -1.0f, -1.0f, 1.0f} },
		{ {-1.0f, -1.0f,  1.0f, 1.0f} },
	};
	Initialise(vertexCount, vertices);
}

void Mesh::InitialiseFromFile(std::string filename)
{
	InitialiseIndexFromFile(filename.c_str(), 0);
}

// TODO: This whole function should probably go (as with the one above), all loading should no longer be done by the mesh and instead the model
void Mesh::InitialiseIndexFromFile(std::string path, int i)
{
	const aiScene* scene = aiImportFile(path.c_str(), Mesh::aiLoadFlag);
	if (!scene) {
		std::cout << "Mesh failed to load at: " << path << "\n";
		return;
	}
	aiMesh* mesh = scene->mMeshes[i];
	
	InitialiseFromAiMesh(path, scene, nullptr, mesh);

	aiReleaseImport(scene);
}

void Mesh::Draw()
{
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

// Only works for files that specify the location of the textures
//std::vector<Texture*> Mesh::LoadMaterialTextures(std::string path, aiMaterial* mat, aiTextureType aiType, Texture::Type type, bool flipOnLoad)
//{
//	std::vector<Texture*> textures;
//	textures.reserve(mat->GetTextureCount(aiType));
//	for (unsigned int i = 0; i < mat->GetTextureCount(aiType); i++)
//	{
//		aiString str;
//		mat->GetTexture(aiType, i, &str);
//		std::string folder = path.substr(0, 1 + path.find_last_of("\\/"));
//		// TODO: Don't default to GL_REPEAT here, change the parameters
//		Texture* texture = ResourceManager::GetTexture(folder + str.C_Str(), type, GL_REPEAT, flipOnLoad);
//		if (texture) {
//			texture->type = type;
//			textures.push_back(texture);
//		}
//		else {
//			std::cout << "Failed attempt to find a texture of path: " << str.C_Str() << "\nThe file might have been moved or renamed\n";
//		}
//	}
//	return textures;
//}

//void Mesh::AddMaterialTextures(std::vector<Texture*>* textures, std::string path, aiMaterial* mat, aiTextureType aiType, Texture::Type type, bool flipOnLoad)
//{
//	std::vector<Texture*> maps = LoadMaterialTextures(path, mat, aiType, type, flipOnLoad);
//	textures->insert(textures->end(), std::make_move_iterator(maps.begin()), std::make_move_iterator(maps.end()));
//}