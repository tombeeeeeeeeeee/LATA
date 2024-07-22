#include "Camera.h"

#include "Maths.h"

#include "imgui.h"

Camera::Camera(glm::vec3 _position, glm::vec3 _up, float _yaw, float _pitch, float _movementSpeed, float _sensitivity, float _fov) :
    position(_position),
    worldUp(_up),
    yaw(_yaw),
    pitch(_pitch),
    movementSpeed(_movementSpeed),
    sensitivity(_sensitivity),
    fov(_fov)
{
    UpdateVectors();
}


Camera::Camera(glm::vec3 _position) : Camera()
{
    position = _position;
    UpdateVectors();
}


Camera::Camera() : Camera({ 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, -90.f, 0.0f, 2.5f, 0.1f, 60.f)
{
    UpdateVectors();
}


// TODO: this could be written without the lookAt function
// TODO: Store postition and rotation in a matrix, could use the Transform class
glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(position, position + front, up); // TODO: Should be the inverse of the camera matrix
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

    // Ensure that when the pitch is out of bounds that the screen does not flip
    if (constrainPitch)
    {
        if (pitch > 89.0f) {
            pitch = 89.0f;
        }
        if (pitch < -89.0f) {
            pitch = -89.0f;
        }
    }

    // Update Front, Right and Up Vectors using the updated Euler angles
    UpdateVectors();
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

void Camera::UpdateVectors()
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

void Camera::GUI()
{
    ImGui::DragFloat3("Position##Camera", &position[0], 0.1f);

    ImGui::BeginDisabled();
    ImGui::DragFloat3("Front##Camera", &front[0]);
    ImGui::DragFloat3("Up##Camera", &up[0]);
    ImGui::DragFloat3("Right##Camera", &right[0]);
    ImGui::EndDisabled();

    // TODO: Should probably also disable this but looks interesting.
    if (ImGui::DragFloat3("World up##Camera", &worldUp[0], 0.1f)) {
        UpdateVectors();
    }

    if (ImGui::DragFloat("Yaw##Camera", &yaw, 0.1f)) {
        UpdateVectors();
    }

    if (ImGui::DragFloat("Pitch##Camera", &pitch, 0.1f)) {
        UpdateVectors();
    }

    ImGui::DragFloat("Movement Speed##Camera", &movementSpeed, 0.1f);
    ImGui::DragFloat("Sensitivity##Camera", &sensitivity, 0.1f);
    ImGui::DragFloat("FOV##Camera", &fov, 0.1f);

    ImGui::DragFloat("Near plane##Camera", &nearPlane, 0.01f, 0.01f, FLT_MAX);
    ImGui::DragFloat("Far plane##Camera", &farPlane, 0.01f, 0.01f, FLT_MAX);
}

toml::table Camera::Serialise()
{
    return toml::table{ 
        { "position", toml::array{ position.x, position.y, position.z } },
        { "rotation", toml::array{ yaw, pitch } },
        { "fov", fov }
    };
}
