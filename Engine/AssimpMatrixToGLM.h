#pragma once
// TODO: Rename this file

#include "Maths.h"
#include "assimp/quaternion.h"
#include "assimp/vector3.h"
#include "assimp/matrix4x4.h"

//TODO: Prob a namespace or class here
glm::mat4 AssimpMatrixToGLM(const aiMatrix4x4& from);

glm::vec3 AssimpVecToGLM(const aiVector3D& vec);

glm::quat AssimpQuatToGLM(const aiQuaternion& pOrientation);