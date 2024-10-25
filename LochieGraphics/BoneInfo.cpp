#include "BoneInfo.h"

#include <iostream>

BoneInfo::BoneInfo()
{
	std::cout << "bone info\n";
	offset = glm::identity<glm::mat4>();
}

BoneInfo::BoneInfo(unsigned int _id, glm::mat4 _offset) :
	ID(_id),
	offset(_offset)
{
}
