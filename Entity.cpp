#include "Entity.h"
#include "gtc/matrix_transform.hpp"

void Entity::Draw()
//model = glm::translate(model, cubePositions[i]);
//float angle = 20.0f * i;
//model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
//lightingShader.setMat4("model", model);
{
	
	glm::mat4 model = glm::mat4(1.f);
	model = glm::translate(model, transform.position);
	model *= transform.scale;
	


}
