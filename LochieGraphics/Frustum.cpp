#include "Frustum.h"

Frustum::Frustum(glm::vec3 pos, float fov, float aspect, float nearClip, float farClip, glm::vec3 up, glm::vec3 forward, glm::vec3 right)
{
    const float halfVSide = farClip * tanf(fov * 0.5f);
    const float halfHSide = halfVSide * aspect;
    const glm::vec3 frontMultFar = farClip * forward;

    glm::vec3 rightNormal = glm::normalize(glm::cross(frontMultFar - right * halfHSide, up));
    glm::vec3 leftNormal = glm::normalize(glm::cross(up, frontMultFar + right * halfHSide));
    glm::vec3 topNormal = glm::normalize(glm::cross(right, frontMultFar - up * halfVSide));
    glm::vec3 bottomNormal = glm::normalize(glm::cross(frontMultFar + up * halfVSide, right));

    nearFace = { forward, glm::dot(pos + nearClip * forward ,forward) };
    farFace = { -forward, glm::dot(pos + frontMultFar, -forward) };
    rightFace = { rightNormal, glm::dot(pos, rightNormal) };
    leftFace = { leftNormal, glm::dot(pos, leftNormal) };
    topFace = { topNormal, glm::dot(pos, topNormal) };
    bottomFace = { bottomNormal, glm::dot(pos, bottomNormal) };
}

bool Frustum::IsOnFrustum(glm::vec3 OOBB[8])
{
    if (!IsInFrontOfPlane(leftFace, OOBB)) return false;
    if (!IsInFrontOfPlane(rightFace, OOBB)) return false;
    if (!IsInFrontOfPlane(topFace, OOBB)) return false;
    if (!IsInFrontOfPlane(bottomFace, OOBB)) return false;
    if (!IsInFrontOfPlane(farFace, OOBB)) return false;
    if (!IsInFrontOfPlane(nearFace, OOBB)) return false;

    return true;
}

bool Frustum::IsInFrontOfPlane(Plane plane, glm::vec3 OOBB[8])
{
    for (int i = 0; i < 8; i++)
    {
        float distance = glm::dot(plane.normal, OOBB[i]);
        if (distance - plane.distance >= 0) return true;
    }
    return false;
}
