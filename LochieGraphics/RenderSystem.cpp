#include "RenderSystem.h"
#include "Maths.h"

void RenderSystem::Update()
{
	// Render depth of scene to texture (from light's perspective)
	glm::mat4 lightSpaceMatrix;
	Light* light = &directionalLight;
	lightSpaceMatrix = light->getShadowViewProjection();
	//Shadow Map moved to init and cached
	shadowMapDepth->Use();
	shadowMapDepth->setMat4("lightSpaceMatrix", lightSpaceMatrix);

	glViewport(0, 0, light->shadowTexWidth, light->shadowTexHeight);
	shadowFrameBuffer->Bind();
	glClear(GL_DEPTH_BUFFER_BIT);
}
