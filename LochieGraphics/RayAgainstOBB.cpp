#include "RayAgainstOBB.h"

bool RayAgainstOBB::RayAgainstOBB(glm::vec3 rayOrigin, glm::vec3 rayDirection, glm::vec3 aabbMin, glm::vec3 aabbMax, glm::mat4 model, float& intersectionDistance)
{
	float tMin = 0.0f;
	float tMax = FLT_MAX;

	glm::vec3 OBBpositionWorldspace(model[3].x, model[3].y, model[3].z);

	glm::vec3 delta = OBBpositionWorldspace - rayOrigin;

	{
		glm::vec3 xAxis(model[0].x, model[0].y, model[0].z);
		float e = glm::dot(xAxis, delta);
		float f = glm::dot(rayDirection, xAxis);
		// Ray basically parallel
		/*if (f < 0.001f && -e + aabbMin.x > 0.0f || -e + aabbMax.x < 0.0f) {
			return false;
		}*/
		float t1 = (e + aabbMax.x) / f;
		float t2 = (e + aabbMin.x) / f;
		if (t1 > t2) {
			// Swap em
			float w = t1;
			t1 = t2;
			t2 = w;
		}
		if (t2 < tMax) {
			tMax = t2;
		}
		if (t1 > tMin) {
			tMin = t1;
		}
		if (tMax < tMin) {
			return false;
		}
	}
	{
		glm::vec3 yAxis(model[1].x, model[1].y, model[1].z);
		float e = glm::dot(yAxis, delta);
		float f = glm::dot(rayDirection, yAxis);
		// Ray basically parallel
		/*if (f < 0.001f && -e + aabbMin.y > 0.0f || -e + aabbMax.y < 0.0f) {
			return false;
		}*/
		float t1 = (e + aabbMax.y) / f;
		float t2 = (e + aabbMin.y) / f;
		if (t1 > t2) {
			// Swap em
			float w = t1;
			t1 = t2;
			t2 = w;
		}
		if (t2 < tMax) {
			tMax = t2;
		}
		if (t1 > tMin) {
			tMin = t1;
		}
		if (tMax < tMin) {
			return false;
		}
	}
	{
		glm::vec3 zAxis(model[2].x, model[2].y, model[2].z);
		float e = glm::dot(zAxis, delta);
		float f = glm::dot(rayDirection, zAxis);
		// Ray basically parallel
		/*if (f < 0.001f && -e + aabbMin.z > 0.0f || -e + aabbMax.z < 0.0f) {
			return false;
		}*/
		float t1 = (e + aabbMax.z) / f;
		float t2 = (e + aabbMin.z) / f;
		if (t1 > t2) {
			// Swap em
			float w = t1;
			t1 = t2;
			t2 = w;
		}
		if (t2 < tMax) {
			tMax = t2;
		}
		if (t1 > tMin) {
			tMin = t1;
		}
		if (tMax < tMin) {
			return false;
		}
	}
	intersectionDistance = tMin;
	return true;
}
