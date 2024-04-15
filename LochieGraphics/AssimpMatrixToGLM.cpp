#include "AssimpMatrixToGLM.h"

glm::mat4 AssimpMatrixToGLM(const aiMatrix4x4& aiM)
{
	glm::mat4 m;
	//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
	m[0][0] = aiM.a1; m[1][0] = aiM.a2; m[2][0] = aiM.a3; m[3][0] = aiM.a4;
	m[0][1] = aiM.b1; m[1][1] = aiM.b2; m[2][1] = aiM.b3; m[3][1] = aiM.b4;
	m[0][2] = aiM.c1; m[1][2] = aiM.c2; m[2][2] = aiM.c3; m[3][2] = aiM.c4;
	m[0][3] = aiM.d1; m[1][3] = aiM.d2; m[2][3] = aiM.d3; m[3][3] = aiM.d4;
	return m;
}

glm::vec3 AssimpVecToGLM(const aiVector3D& vec)
{
	return glm::vec3(vec.x, vec.y, vec.z);
}

glm::quat AssimpQuatToGLM(const aiQuaternion& pOrientation)
{
	return glm::quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
}


