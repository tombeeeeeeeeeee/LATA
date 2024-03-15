#include "Camera.h"


#include "glm/gtc/matrix_transform.hpp"


Camera::Camera()
{
    SetDefaults();
    position = glm::vec3(0.f, 0.f, 0.f);
    worldUp = glm::vec3(0.f, 1.f, 0.f);
    yaw = 0.f;
    pitch = 0.f;
    updateVectors();
}

Camera::Camera(glm::vec3 _position, glm::vec3 _up, float _yaw, float _pitch)
{
    SetDefaults();
    position = _position;
    worldUp = _up;
    yaw = _yaw;
    pitch = _pitch;
    updateVectors();
}

Camera::Camera(glm::vec3 _position)
{
    SetDefaults();
    position = _position;
    updateVectors();
}


void Camera::SetDefaults()
{
    yaw = defaultYaw;
    pitch = defaultPitch;
    worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    movementSpeed = defaultMoveSpeed;
    sensitivity = defaultSensitivity;
    fov = defaultFov;
}

glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(position, position + front, up);
}

void Camera::ProcessKeyboard(Direction direction, float deltaTime)
{
    float velocity = movementSpeed * deltaTime;
    switch (direction)
    {
    case Camera::FORWARD:
        position += front * velocity;
        break;
    case Camera::BACKWARD:
        position -= front * velocity;
        break;
    case Camera::LEFT:
        position -= right * velocity;
        break;
    case Camera::RIGHT:
        position += right * velocity;
        break;
    case Camera::UP:
        position += worldUp * velocity;
        //position += up * velocity;
        break;
    case Camera::DOWN:
        position -= worldUp * velocity;
        //position -= up * velocity;
        break;
    }
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (pitch > 89.0f) {
            pitch = 89.0f;
        }
        if (pitch < -89.0f) {
            pitch = -89.0f;
        }
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    updateVectors();

}


void Camera::ProcessMouseScroll(float yoffset)
{
    fov -= static_cast<float>(yoffset);
    if (fov < 10.0f) {
        fov = 10.0f;
    }
    else if (fov > 170.0f) {
        fov = 170.0f;
    }
}

void Camera::updateVectors()
{
    // calculate the new Front vector
    front = glm::normalize(
        glm::vec3(  cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
                    sin(glm::radians(pitch)),
                    sin(glm::radians(yaw)) * cos(glm::radians(pitch))));
    // also re-calculate the Right and Up vector
    right = glm::normalize(glm::cross(front, worldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    up = glm::normalize(glm::cross(right, front));

}