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

	float nearPlane = 1.0f;
	float farPlane = 1000.0f;

	enum State {
		editorMode,
		targetingPositionOrthographic,
		targetingPositionPerspective,
		targetingPlayersOrthographic,
		targetingPlayersPerspective,
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
	ArtState artState = none;
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
	glm::mat4 GetProjectionMatrix(float aspectRatio) const;

	// processes keyboard input. Accepts input parameter in the form of camera defined ENUM to abstract it from windowing systems
	void ProcessKeyboard(Direction direction, float deltaTime);

	// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch);


	// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(float yoffset);

	float getOrthoHeight(float ratio) const;
	float getOrthoWidth(float ratio) const;

	bool InOrthoMode() const;

	void Rotate(float x, float y);

	void GUI();

	toml::table Serialise() const;

};

