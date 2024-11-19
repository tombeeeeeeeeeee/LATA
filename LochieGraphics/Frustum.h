#include "Maths.h"


struct Plane
{
	glm::vec3 normal;
	float distance;
};

struct Frustum
{
	Plane topFace;
	Plane bottomFace;

	Plane rightFace;
	Plane leftFace;

	Plane farFace;
	Plane nearFace;

	Frustum() {};
	Frustum(glm::vec3 pos, float fov, float aspect, float nearClip, float farClip,
		glm::vec3 up, glm::vec3 forward, glm::vec3 right);
	bool IsOnFrustum(glm::vec3 OOBB[8]);
	bool IsOnFrustum(glm::vec3 point);
	bool IsOnFrustum(glm::vec3 centre, float radius);
	bool IsInFrontOfPlane(Plane plane ,glm::vec3 OOBB[8]);
	bool IsInFrontOfPlane(Plane plane ,glm::vec3 point);
	bool IsInFrontOfPlane(Plane plane, glm::vec3 centre, float radius);
};