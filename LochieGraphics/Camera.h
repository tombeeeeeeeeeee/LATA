#pragma once

#include "Maths.h"
#include "Graphics.h"

#include <vector>

// TODO: Make this fit better with the sceneObject system
class Camera
{
public:
    // TODO: Change to matrix transform? use transform class
    // camera Attributes
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    // euler Angles
    //TODO: Roll
    float yaw;
    float pitch;
    // camera options
    float movementSpeed;
    float sensitivity;
    float fov;

    enum Direction {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        UP,
        DOWN
    };

    Camera();
    Camera(glm::vec3 _position, glm::vec3 _up, float _yaw, float _pitch, float _movementSpeed, float _sensitivity, float _fov);
    Camera(glm::vec3 _position);

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix

    glm::mat4 GetViewMatrix() const;

    // processes keyboard input. Accepts input parameter in the form of camera defined ENUM to abstract it from windowing systems
    void ProcessKeyboard(Direction direction, float deltaTime);

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch);

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset);

    void GUI();
private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void UpdateVectors();

};

