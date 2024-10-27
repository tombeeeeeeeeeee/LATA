#pragma once

#include "Transform.h"

namespace toml {
    inline namespace v3 {
        class table;
    }
}

// TODO: Make this fit better with the sceneObject system
class Camera
{
public:
    Transform transform;

    float fov;

    float nearPlane;
    float farPlane;

    enum State {
        editorMode,
        targetingPosition,
        targetingPlayers,
        artEditorMode,
        tilePlacing
    };

    State state = editorMode;
    float orthoScale = 1.0f;
    bool editorOrth = false;

    bool editorRotate = false;



    bool artKeyDown = false;

    // TODO: Move art stuff into seperate camera system
    enum ArtState {
        none, // No movement
        orbit, // Orbit
        dolly, // Forward/backward
        boomTruck, // Left/Right/Up/Down locally
    };
    ArtState artState;
    float artFocusDistance = 1;

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

    // TODO: This shouldn't be hardcoded
    float getOrthoHeight() const { return 9.0f * orthoScale; };
    float getOrthoWidth() const { return 16.0f * orthoScale; };

    bool InOrthoMode() const;

    void Rotate(float x, float y);

    void GUI();

    toml::table Serialise() const;

};

