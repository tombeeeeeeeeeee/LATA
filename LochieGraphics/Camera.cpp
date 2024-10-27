#include "Camera.h"

#include "UserPreferences.h"

#include "EditorGUI.h"
#include "Serialisation.h"


Camera::Camera(glm::vec3 _position, glm::vec3 _up, float _yaw, float _pitch, float _movementSpeed, float _sensitivity, float _fov) :
    transform(nullptr, _position),
    fov(_fov)
{
    UserPreferences::camMove = _movementSpeed;
    UserPreferences::camRotate = _sensitivity;
}


Camera::Camera(glm::vec3 _position) : Camera()
{
    transform.setPosition(_position);
}


Camera::Camera() : Camera({ 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, -0.1f, 0.0f, 2.5f, 0.01f, 60.0f)
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
    if (state == editorMode) {
        float velocity = UserPreferences::camMove * deltaTime;
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
    else if (state == tilePlacing) {
        float velocity = UserPreferences::camMove * deltaTime;
        switch (direction)
        {
        case Camera::FORWARD:  transform.setPosition(transform.getPosition() + transform.up() * velocity);    break;
        case Camera::BACKWARD: transform.setPosition(transform.getPosition() - transform.up() * velocity);    break;
        case Camera::LEFT:     transform.setPosition(transform.getPosition() - transform.right() * velocity); break;
        case Camera::RIGHT:    transform.setPosition(transform.getPosition() + transform.right() * velocity); break;
        }
    }
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{

    if (state == editorMode && editorRotate) {
        Rotate(xoffset * UserPreferences::camRotate, yoffset * UserPreferences::camRotate);
    }
    else if (state == artEditorMode && artKeyDown) {
        if (artState == orbit) {
            transform.setPosition(transform.getPosition() + transform.forward() * artFocusDistance);
            Rotate(xoffset * UserPreferences::camOrbit, yoffset * UserPreferences::camOrbit);
            transform.setPosition(transform.getPosition() - transform.forward() * artFocusDistance);
        }
        else if (artState == dolly) {
            float moveAmount = UserPreferences::camMoveDolly * (xoffset + yoffset);
            artFocusDistance -= moveAmount;
            transform.setPosition(transform.getPosition() + transform.forward() * moveAmount);
        }
        else if (artState == boomTruck) {
            transform.setPosition(transform.getPosition() - (transform.up() * yoffset * UserPreferences::camBoomTruck) - (transform.right() * xoffset * UserPreferences::camBoomTruck));
        }
    }
}


void Camera::ProcessMouseScroll(float yoffset)
{
    if (state == editorMode && !editorOrth) {
        fov -= static_cast<float>(yoffset);
        if (fov < 10.0f) {
            fov = 10.0f;
        }
        else if (fov > 170.0f) {
            fov = 170.0f;
        }
    }
    else if (state == artEditorMode) {
        float moveAmount = UserPreferences::camScrollDolly * yoffset;
        artFocusDistance -= moveAmount;
        transform.setPosition(transform.getPosition() + transform.forward() * moveAmount);
    }
    else if (InOrthoMode()) {
        orthoScale -= yoffset * UserPreferences::orthScrollSpeed;
        if (orthoScale < 0.1f) {
            orthoScale = 0.1f;
        }
    }
}

void Camera::GUI()
{
    transform.GUI();

    ImGui::DragFloat("FOV##Camera", &fov, 0.1f);

    ImGui::DragFloat("Near plane##Camera", &nearPlane, 0.01f, 0.01f, FLT_MAX);
    ImGui::DragFloat("Far plane##Camera", &farPlane, 0.01f, 0.01f, FLT_MAX);

    ImGui::Combo("Mode", (int*)&state, "Editor\0Targeting Position\0Targeting Players\0Art\0Tile Placing\0");
    if (state == editorMode) {
        ImGui::Checkbox("Orthographic Mode", &editorOrth);
    }
    if (InOrthoMode()) {
        ImGui::DragFloat("Orthographic Scale##Camera", &orthoScale, 0.01f, 0.01f, FLT_MAX);
    }
}

bool Camera::InOrthoMode() const
{
    return state == State::targetingPlayers || 
        state == State::targetingPosition || 
        (state == State::editorMode && editorOrth) ||
        state == State::tilePlacing;
}

void Camera::Rotate(float x, float y)
{
    glm::vec3 euler = { 0.0f, -x, y };

    glm::vec3 rotationEuler = glm::vec3(euler.x, euler.y, euler.z);

    glm::quat quatZ = glm::angleAxis(rotationEuler.z, transform.right());
    glm::quat quatY = glm::angleAxis(rotationEuler.y, glm::vec3(0, 1, 0));
    glm::quat quatX = glm::angleAxis(rotationEuler.x, glm::vec3(1, 0, 0));

    transform.setRotation(glm::normalize(quatX * quatY * quatZ) * transform.getRotation());
}

toml::table Camera::Serialise() const
{
    // TODO: Fix
    // TODO: Transform
    // Move speeds
    return toml::table{ 
        //{ "movementSpeed", movementSpeed },
        //{ "sensitivity", sensitivity },
        { "fov", fov },
        { "near", nearPlane },
        { "far", farPlane },
        { "state", (int)state},
        { "orthoScale", orthoScale},
    };
}
