#include "Camera.h"

#include "Maths.h"

#include "imgui.h"

Camera::Camera(glm::vec3 _position, glm::vec3 _up, float _yaw, float _pitch, float _movementSpeed, float _sensitivity, float _fov) :
    transform(nullptr, _position),
    movementSpeed(_movementSpeed),
    sensitivity(_sensitivity),
    fov(_fov)
{
    // TODO: yaw pitch
    UpdateVectors();
}


Camera::Camera(glm::vec3 _position) : Camera()
{
    transform.setPosition(_position);
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
    return glm::lookAt(transform.getPosition(), transform.getPosition() + transform.forward(), transform.up()); // TODO: Should be the inverse of the camera matrix

    //return glm::inverse(transform.getGlobalMatrix());
    //return glm::lookAt(position, position + front, up); // TODO: Should be the inverse of the camera matrix
}

void Camera::ProcessKeyboard(Direction direction, float deltaTime)
{
    float velocity = movementSpeed * deltaTime;
    switch (direction)
    {
    case Camera::FORWARD:
        transform.setPosition(transform.getPosition() + transform.forward() * velocity);
        break;
    case Camera::BACKWARD:
        transform.setPosition(transform.getPosition() - transform.forward() * velocity);
        break;
    case Camera::LEFT:
        transform.setPosition(transform.getPosition() - transform.right() * velocity);
        break;
    case Camera::RIGHT:
        transform.setPosition(transform.getPosition() + transform.right() * velocity);
        break;
    case Camera::UP:
        transform.setPosition(transform.getPosition() + glm::vec3(0.0f, 1.0f, 0.0f) * velocity);
        break;
    case Camera::DOWN:
        transform.setPosition(transform.getPosition() - glm::vec3(0.0f, 1.0f, 0.0f) * velocity);
        break;
    }
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
    xoffset *= sensitivity;
    yoffset *= sensitivity;


    glm::vec3 euler = transform.getEulerRotation();
    euler.y + xoffset;
    euler.z + yoffset;

    // TODO: this should be a transform function
    euler -= transform.getEulerRotation();

    euler = { 0.0f, -xoffset, yoffset };

    glm::vec3 rotationEuler = glm::vec3(glm::radians(euler.x), glm::radians(euler.y), glm::radians(euler.z));

    glm::quat quatZ = glm::angleAxis(rotationEuler.z, transform.right());
    glm::quat quatY = glm::angleAxis(rotationEuler.y, glm::vec3(0, 1, 0));
    glm::quat quatX = glm::angleAxis(rotationEuler.x, glm::vec3(1, 0, 0));

    transform.setRotation(glm::normalize(quatZ * quatY * quatX) * transform.getRotation());


    //transform.setEulerRotation({old.x, old.y + xoffset, old.z + yoffset});

    // Ensure that when the pitch is out of bounds that the screen does not flip
    if (constrainPitch)
    {
        // TODO: Fix
        //if (pitch > 89.0f) {
        //    pitch = 89.0f;
        //}
        //if (pitch < -89.0f) {
        //    pitch = -89.0f;
        //}
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
    //// calculate the new Front vector
    //front = glm::normalize(
    //    glm::vec3(  cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
    //                sin(glm::radians(pitch)),
    //                sin(glm::radians(yaw)) * cos(glm::radians(pitch))));
    //// also re-calculate the Right and Up vector
    //right = glm::normalize(glm::cross(front, worldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    //up = glm::normalize(glm::cross(right, front));
}

void Camera::GUI()
{
    transform.GUI();
    //ImGui::DragFloat3("Position##Camera", &position[0], 0.1f);

    //ImGui::BeginDisabled();
    //ImGui::DragFloat3("Front##Camera", &front[0]);
    //ImGui::DragFloat3("Up##Camera", &up[0]);
    //ImGui::DragFloat3("Right##Camera", &right[0]);
    //ImGui::EndDisabled();

    // TODO: Should probably also disable this but looks interesting.
   // if (ImGui::DragFloat3("World up##Camera", &worldUp[0], 0.1f)) {
    //    UpdateVectors();
    //}

    //if (ImGui::DragFloat("Yaw##Camera", &yaw, 0.1f)) {
    //    UpdateVectors();
    //}

    //if (ImGui::DragFloat("Pitch##Camera", &pitch, 0.1f)) {
    //    UpdateVectors();
    //}

    ImGui::DragFloat("Movement Speed##Camera", &movementSpeed, 0.1f);
    ImGui::DragFloat("Sensitivity##Camera", &sensitivity, 0.1f);
    ImGui::DragFloat("FOV##Camera", &fov, 0.1f);

    ImGui::DragFloat("Near plane##Camera", &nearPlane, 0.01f, 0.01f, FLT_MAX);
    ImGui::DragFloat("Far plane##Camera", &farPlane, 0.01f, 0.01f, FLT_MAX);
}

toml::table Camera::Serialise()
{
    // TODO: Fix
    //return toml::table{ 
    //    { "position", toml::array{ position.x, position.y, position.z } },
    //    { "rotation", toml::array{ yaw, pitch } },
    //    { "fov", fov }
    //};
    return toml::table{};
}
