#pragma once

class SceneObject;

class Part
{
public:
	SceneObject* sceneObject;

	virtual void GUI() {};

	virtual void Update() {};
	virtual void Draw() {};
};