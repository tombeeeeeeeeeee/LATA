#include "Camera.h"

#include "Maths.h"

#include "imgui.h"

Camera::Camera(glm::vec3 _position, glm::vec3 _up, float _yaw, float _pitch, float _movementSpeed, float _sensitivity, float _fov) :
    transform(nullptr, _position),
    movementSpeed(_movementSpeed),
    sensitivity(_sensitivity),
    fov(_fov)
{
}


Camera::Camera(glm::vec3 _position) : Camera()
{
    transform.setPosition(_position);
}


Camera::Camera() : Camera({ 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, -90.f, 0.0f, 2.5f, 0.1f, 60.f)
{
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

    if (state == editorMode) {
        switch (direction)
        {
        case Camera::FORWARD:  transform.setPosition(transform.getPosition() + transform.forward() * velocity);         break;
        case Camera::BACKWARD: transform.setPosition(transform.getPosition() - transform.forward() * velocity);         break;
        case Camera::LEFT:     transform.setPosition(transform.getPosition() - transform.right() * velocity);           break;
        case Camera::RIGHT:    transform.setPosition(transform.getPosition() + transform.right() * velocity);           break;
        case Camera::UP:       transform.setPosition(transform.getPosition() + glm::vec3(0.0f, 1.0f, 0.0f) * velocity); break;
        case Camera::DOWN:     transform.setPosition(transform.getPosition() - glm::vec3(0.0f, 1.0f, 0.0f) * velocity); break;
        }
    }
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    if (state == editorMode && editorRotate) {
        Rotate(xoffset, yoffset);
    }
    else if (state == artEditorMode && artKeyDown) {
        if (artState == orbit) {
            transform.setPosition(transform.getPosition() + transform.forward() * artFocusDistance);
            Rotate(xoffset, yoffset);
            transform.setPosition(transform.getPosition() - transform.forward() * artFocusDistance);
        }
        else if (artState == dolly) {
            float moveAmount = movementSpeed * (xoffset + yoffset);
            artFocusDistance -= moveAmount;
            transform.setPosition(transform.getPosition() + transform.forward() * movementSpeed * (xoffset + yoffset));
        }
        else if (artState == boomTruck) {
            transform.setPosition(transform.getPosition() + (transform.up() * yoffset) + (transform.right() * xoffset));
        }
    }
}


void Camera::ProcessMouseScroll(float yoffset)
{
    if (state == editorMode) {
        fov -= static_cast<float>(yoffset);
        if (fov < 10.0f) {
            fov = 10.0f;
        }
        else if (fov > 170.0f) {
            fov = 170.0f;
        }
    }
    else if (state == artEditorMode) {
        float moveAmount = movementSpeed * yoffset;
        artFocusDistance -= moveAmount;
        transform.setPosition(transform.getPosition() + transform.forward() * movementSpeed * yoffset);
    }
}

bool Camera::InEditorMode() const {
    return state == editorMode || state == artEditorMode;
}

void Camera::GUI()
{
    transform.GUI();

    ImGui::DragFloat("Movement Speed##Camera", &movementSpeed, 0.1f);
    ImGui::DragFloat("Sensitivity##Camera", &sensitivity, 0.1f);
    ImGui::DragFloat("FOV##Camera", &fov, 0.1f);

    ImGui::DragFloat("Near plane##Camera", &nearPlane, 0.01f, 0.01f, FLT_MAX);
    ImGui::DragFloat("Far plane##Camera", &farPlane, 0.01f, 0.01f, FLT_MAX);

    ImGui::DragFloat("Orthographic Scale##Camera", &orthoScale, 0.01f, 0.01f, FLT_MAX);

    ImGui::Combo("Combo", (int*)&state, "Editor\0Targeting Position\0Targeting Players\0Art\0");
}

void Camera::Rotate(float x, float y)
{
    glm::vec3 euler = { 0.0f, -x, y };

    glm::vec3 rotationEuler = glm::vec3(glm::radians(euler.x), glm::radians(euler.y), glm::radians(euler.z));

    glm::quat quatZ = glm::angleAxis(rotationEuler.z, transform.right());
    glm::quat quatY = glm::angleAxis(rotationEuler.y, glm::vec3(0, 1, 0));
    glm::quat quatX = glm::angleAxis(rotationEuler.x, glm::vec3(1, 0, 0));

    transform.setRotation(glm::normalize(quatX * quatY * quatZ) * transform.getRotation());
}

toml::table Camera::Serialise()
{
    // TODO: Fix
    // TODO: Transform
    return toml::table{ 
        { "movementSpeed", movementSpeed },
        { "fov", fov },
        { "sensitivity", sensitivity },
        { "near", nearPlane },
        { "far", farPlane },
        { "state", (int)state},
        { "orthoScale", orthoScale},
    };
}
