#include "Entity.h"
#include "gtc/matrix_transform.hpp"

void Entity::Draw()
{
	glm::mat4 model = glm::mat4(transform.scale);
	model = glm::translate(model, transform.position);
	meshRenderer->material.shader->setMat4("model", model);
	
	meshRenderer->Draw();


}
